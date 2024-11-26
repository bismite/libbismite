#include <stdbool.h>
#include <stdio.h>
#include <bi/shader.h>
#include <bi/camera.h>
#include <bi/util.h>
#include <bi/bi_sdl.h>
#include <bi/texture.h>
#include <bi/node.h>
#include <bi/shader_node.h>
#include <bi/framebuffer.h>
#include "matrix/matrix.h"

static void print_shader_log(GLuint shader_id)
{
  int len=0;
  glGetShaderiv(shader_id,GL_INFO_LOG_LENGTH,&len);
  if(len>0){
    char *log = malloc(len*sizeof(char));
    glGetShaderInfoLog(shader_id, len, NULL, log);
    printf("glGetShaderInfoLog(): %s\n",log);
    free(log);
  }
}
static void print_program_log(GLuint program_id)
{
  int len=0;
  glGetProgramiv(program_id,GL_INFO_LOG_LENGTH,&len);
  if(len>0){
    char *log = malloc(len*sizeof(char));
    glGetProgramInfoLog(program_id, len, NULL, log);
    printf("glGetProgramInfoLog(): %s\n",log);
    free(log);
  }
}

static inline GLuint create_shader(GLenum shader_type, const char* src) {
  GLuint shader = glCreateShader(shader_type);
  if(shader==0) {
    printf("glCreateShader() failed.\n");
  }
  glShaderSource(shader, 1, &src, NULL);
  glCompileShader(shader);
  GLint compiled = 0;
  glGetShaderiv(shader,GL_COMPILE_STATUS,&compiled);
  if( compiled != GL_TRUE ) {
    printf("glCompileShader failed.\n");
    if(shader_type==GL_VERTEX_SHADER)   printf("GL_SHADER_TYPE = GL_VERTEX_SHADER\n");
    if(shader_type==GL_FRAGMENT_SHADER) printf("GL_SHADER_TYPE = GL_FRAGMENT_SHADER\n");
    print_shader_log(shader);
  }
  return shader;
}

static void load_shader(BiShader* shader,const char* vertex_shader_source, const char* fragment_shader_source)
{
  GLuint vert_shader = create_shader(GL_VERTEX_SHADER,vertex_shader_source);
  GLuint frag_shader = create_shader(GL_FRAGMENT_SHADER,fragment_shader_source);
  GLuint program_id = glCreateProgram();
  glAttachShader(program_id,vert_shader);
  glDeleteShader(vert_shader);
  glAttachShader(program_id,frag_shader);
  glDeleteShader(frag_shader);
  glLinkProgram(program_id);
  GLint link_status = 0;
  glGetProgramiv(program_id, GL_LINK_STATUS, &link_status);
  if(link_status!=GL_TRUE){
    printf("glLinkProgram() failed.\n");
    print_program_log(program_id);
  }
  //
  shader->program_id = program_id;
  glUseProgram(program_id);
  // Attribute locations
  shader->attribute.vertex = glGetAttribLocation(program_id, "vertex");
  shader->attribute.texture_uv = glGetAttribLocation(program_id, "texture_uv");
  shader->attribute.texture_crop_uv = glGetAttribLocation(program_id, "texture_crop_uv");
  shader->attribute.texture_index = glGetAttribLocation(program_id, "texture_index");
  shader->attribute.tint = glGetAttribLocation(program_id, "tint");
  shader->attribute.modulate = glGetAttribLocation(program_id, "modulate");
  shader->attribute.node_size = glGetAttribLocation(program_id, "node_size");
  shader->attribute.transform = glGetAttribLocation(program_id, "transform");
  shader->attribute.node_extra_data = glGetAttribLocation(program_id, "node_extra_data");
  // Uniform Locations
  shader->uniform.camera = glGetUniformLocation(program_id, "camera");
  shader->uniform.texture = glGetUniformLocation(program_id, "sampler[0]");
  shader->uniform.time = glGetUniformLocation(program_id, "time");
  shader->uniform.resolution = glGetUniformLocation(program_id, "resolution");
  shader->uniform.viewport_size = glGetUniformLocation(program_id, "viewport_size");
  shader->uniform.scale = glGetUniformLocation(program_id, "scale");
  shader->uniform.shader_extra_data = glGetUniformLocation(program_id, "shader_extra_data");
  glUseProgram(0);
}

static inline void init_mat4_buffer(GLint location, GLuint buffer)
{
  if( location < 0 ) return;
  for(int i=0;i<4;i++){
    glBindBuffer(GL_ARRAY_BUFFER,buffer);
    glEnableVertexAttribArray(location+i );
    GLsizei stride = sizeof(GLfloat)*4*4;
    void* position = NULL+sizeof(GLfloat)*4*i;
    glVertexAttribPointer(location+i,4,GL_FLOAT,GL_FALSE,stride,position);
    glVertexAttribDivisor(location+i, 1);
  }
}

static inline void generate_vao(GLuint buf, GLuint attr, GLint size, GLuint div)
{
  glBindBuffer(GL_ARRAY_BUFFER,buf);
  glEnableVertexAttribArray(attr);
  glVertexAttribPointer(attr,size,GL_FLOAT,GL_FALSE,0,NULL);
  glVertexAttribDivisor(attr, div);
}
static inline void generate_vao_int(GLuint buf, GLuint attr, GLint size, GLuint div)
{
  glBindBuffer(GL_ARRAY_BUFFER,buf);
  glEnableVertexAttribArray(attr);
  glVertexAttribIPointer(attr,size,GL_INT,0,NULL);
  glVertexAttribDivisor(attr, div);
}

void bi_shader_init(BiShader* shader,const char* vertex_shader_source, const char* fragment_shader_source)
{
  load_shader(shader,vertex_shader_source,fragment_shader_source);

  // set unit to sampler2D
  glUseProgram(shader->program_id);
  int texture_location_indexes[BI_SHADER_MAX_TEXTURES];
  for(int i=0; i<BI_SHADER_MAX_TEXTURES; i++) { texture_location_indexes[i] = i; }
  glUniform1iv(shader->uniform.texture, BI_SHADER_MAX_TEXTURES, texture_location_indexes);
  glUseProgram(0);

  // create vbo
  glGenBuffers(1, &shader->buffer.texture_uv);
  glGenBuffers(1, &shader->buffer.texture_crop_uv);
  glGenBuffers(1, &shader->buffer.texture_index);
  glGenBuffers(1, &shader->buffer.tint);
  glGenBuffers(1, &shader->buffer.modulate);
  glGenBuffers(1, &shader->buffer.transform);
  glGenBuffers(1, &shader->buffer.node_size);
  glGenBuffers(1, &shader->buffer.node_extra_data);

  // create vbo : vertex
  glGenBuffers(1, &shader->buffer.vertex);
  glBindBuffer(GL_ARRAY_BUFFER, shader->buffer.vertex);
  GLfloat l=0, t=1, r=1, b=0; // vertex left,top,right,bottom
  GLfloat vertex[8] = { l,t, l,b, r,t, r,b };
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 8, vertex, GL_DYNAMIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  // generate vao
  glGenVertexArrays(1, &shader->vao);
  glBindVertexArray(shader->vao);
    generate_vao(shader->buffer.vertex, shader->attribute.vertex, 2, 0);
    generate_vao(shader->buffer.texture_uv, shader->attribute.texture_uv, 4, 1);
    generate_vao(shader->buffer.texture_crop_uv, shader->attribute.texture_crop_uv, 4, 1);
    generate_vao_int(shader->buffer.texture_index,shader->attribute.texture_index,1,1);
    generate_vao(shader->buffer.tint, shader->attribute.tint, 4, 1);
    generate_vao(shader->buffer.modulate, shader->attribute.modulate, 4, 1);
    generate_vao(shader->buffer.node_size, shader->attribute.node_size, 2, 1);
    init_mat4_buffer(shader->attribute.transform, shader->buffer.transform);
    init_mat4_buffer(shader->attribute.node_extra_data, shader->buffer.node_extra_data);
  glBindVertexArray(0); // unbind vao
}

void bi_shader_set_uniforms(BiShader* shader,double time,BiFramebuffer* fb,BiShaderNode* snode)
{
  glUniform1f( shader->uniform.time, time);
  glUniform2f( shader->uniform.resolution, fb->w,fb->h );
  glUniform2f( shader->uniform.viewport_size, fb->viewport_w,fb->viewport_h );
  glUniformMatrix4fv(shader->uniform.shader_extra_data, 1, GL_FALSE, snode->shader_extra_data );
}

static inline void update_vbo(GLuint buffer,size_t len, const void *data)
{
  // orphaning: https://www.khronos.org/opengl/wiki/Buffer_Object_Streaming#Buffer_re-specification
  glBindBuffer(GL_ARRAY_BUFFER, buffer);
  glBufferData(GL_ARRAY_BUFFER, len, NULL, GL_DYNAMIC_DRAW);
  glBufferSubData(GL_ARRAY_BUFFER, 0, len, data);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void bi_shader_draw(BiShader* shader,Array* queue)
{
  const size_t len = queue->size;
  static size_t len_max = 0;
  static GLfloat *transforms = NULL;
  static GLfloat *texture_uv = NULL;
  static GLfloat *texture_crop_uv = NULL;
  static GLint *texture_index = NULL;
  static GLfloat* tint = NULL;
  static GLfloat* modulate = NULL;
  static GLfloat* node_size = NULL;
  static GLfloat *extra_data = NULL;

  if(len>len_max){
    len_max = len;
    transforms = realloc( transforms, sizeof(GLfloat)*len*16 );
    texture_uv = realloc( texture_uv, sizeof(GLfloat)*len*4 );
    texture_crop_uv = realloc( texture_crop_uv, sizeof(GLfloat)*len*4 );
    texture_index = realloc( texture_index, sizeof(GLint)*len );
    tint = realloc( tint, sizeof(GLfloat)*4*len );
    modulate = realloc( modulate, sizeof(GLfloat)*4*len );
    node_size = realloc( node_size, sizeof(GLfloat)*2*len );
    extra_data = realloc( extra_data, sizeof(GLfloat)*len*16 );
  }

  for( uint64_t i=0; i<len; i++ ) {
    BiNode* node = queue->objects[i];
    // Matrix
    bi_mat4_copy(&transforms[i*16], node->draw_matrix);
    // Texture
    if(bi_node_get_texture(node) != NULL) {
      for(int j=0;j<4;j++){
        texture_uv[i*4+j] = node->texture_uv[j];
        texture_crop_uv[i*4+j] = node->texture_crop_uv[j];
      }
      if(node->texture_flip_horizontal) {
        texture_uv[i*4+0] = node->texture_uv[2]; // left = right
        texture_uv[i*4+2] = node->texture_uv[0]; // right = left
      }
      if( node->texture_flip_vertical) {
        texture_uv[i*4+1] = node->texture_uv[3]; // bottom = top
        texture_uv[i*4+3] = node->texture_uv[1]; // top = bottom
      }
      texture_index[i] = bi_node_get_texture(node)->_texture_unit;
    }else{
      texture_index[i] = -1; // no-texture
    }
    // colors
    tint[i*4+0] = node->tint.r;
    tint[i*4+1] = node->tint.g;
    tint[i*4+2] = node->tint.b;
    tint[i*4+3] = node->tint.a;
    modulate[i*4+0] = node->color.r;
    modulate[i*4+1] = node->color.g;
    modulate[i*4+2] = node->color.b;
    modulate[i*4+3] = node->color.a;
    // node size
    node_size[i*2+0] = node->w;
    node_size[i*2+1] = node->h;
    // extra_data
    bi_mat4_copy(&extra_data[i*16], node->shader_extra_data);
  }

  // update vbo
  update_vbo(shader->buffer.transform, sizeof(GLfloat)*16*len, transforms);
  update_vbo(shader->buffer.texture_index, sizeof(GLint)*1*len, texture_index);
  update_vbo(shader->buffer.texture_uv, sizeof(GLfloat)*4*len, texture_uv);
  update_vbo(shader->buffer.texture_crop_uv, sizeof(GLfloat)*4*len, texture_crop_uv);
  update_vbo(shader->buffer.tint, sizeof(GLfloat)*4*len, tint);
  update_vbo(shader->buffer.modulate, sizeof(GLfloat)*4*len, modulate);
  update_vbo(shader->buffer.node_size, sizeof(GLfloat)*2*len, node_size);
  update_vbo(shader->buffer.node_extra_data, sizeof(GLfloat)*16*len, extra_data);
  // Draw instances
  glBindVertexArray(shader->vao);
    glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, len);
  glBindVertexArray(0);
}
