#include <stdbool.h>
#include <stdio.h>
#include <bi/shader.h>
#include <bi/camera.h>
#include <bi/util.h>
#include <bi/bi_sdl.h>
#include <bi/texture.h>
#include <bi/node.h>
#include "matrix/matrix.h"

static void print_shader_log(const char* name, GLuint shader_id)
{
  int loglen=0;
  glGetShaderiv(shader_id,GL_INFO_LOG_LENGTH,&loglen);
  if(loglen>0){
    char *log = malloc(loglen);
    glGetShaderInfoLog(shader_id, loglen, NULL, log);
    printf("%s: %s\n",name,log);
    free(log);
  }
}

static void load_shader(BiShader* shader,const char* vertex_shader_source, const char* fragment_shader_source)
{
  // compile vertex shader
  GLuint vShaderId = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vShaderId, 1, &vertex_shader_source, NULL);
  glCompileShader(vShaderId);
  print_shader_log("Vertex Shader Log",vShaderId);

  // compile fragment shader
  GLuint fShaderId = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fShaderId, 1, &fragment_shader_source, NULL);
  glCompileShader(fShaderId);
  print_shader_log("Fragment Shader Log",fShaderId);

  //
  GLuint program_id = glCreateProgram();
  glAttachShader(program_id,vShaderId);
  glDeleteShader(vShaderId);
  glAttachShader(program_id,fShaderId);
  glDeleteShader(fShaderId);
  glLinkProgram(program_id);

  //
  shader->program_id = program_id;

  // Attribute locations
  shader->attribute.vertex = glGetAttribLocation(program_id, "vertex");
  shader->attribute.texture_uv = glGetAttribLocation(program_id, "texture_uv");
  shader->attribute.texture_index = glGetAttribLocation(program_id, "texture_index");
  shader->attribute.opacity = glGetAttribLocation(program_id, "opacity");
  shader->attribute.color[0] = glGetAttribLocation(program_id, "color0");
  shader->attribute.color[1] = glGetAttribLocation(program_id, "color1");
  shader->attribute.transform = glGetAttribLocation(program_id, "transform");
  shader->attribute.node_extra_data = glGetAttribLocation(program_id, "node_extra_data");

  // Uniform
  shader->uniform.camera = glGetUniformLocation(program_id, "camera");
  shader->uniform.texture = glGetUniformLocation(program_id, "sampler[0]");
  shader->uniform.time = glGetUniformLocation(program_id, "time");
  shader->uniform.resolution = glGetUniformLocation(program_id, "resolution");
  shader->uniform.scale = glGetUniformLocation(program_id, "scale");
  shader->uniform.layer_extra_data = glGetUniformLocation(program_id, "layer_extra_data");
}

void bi_shader_init(BiShader* shader,const char* vertex_shader_source, const char* fragment_shader_source)
{
  load_shader(shader,vertex_shader_source,fragment_shader_source);

  // set unit to sampler2D
  glUseProgram(shader->program_id);
  int texture_location_indexes[BI_LAYER_MAX_TEXTURES];
  for(int i=0; i<BI_LAYER_MAX_TEXTURES; i++) { texture_location_indexes[i] = i; }
  glUniform1iv(shader->uniform.texture, BI_LAYER_MAX_TEXTURES, texture_location_indexes);
  glUseProgram(0);

  // create vbo
  glGenBuffers(1, &shader->buffer.uv);
  glGenBuffers(1, &shader->buffer.texture_index);
  glGenBuffers(1, &shader->buffer.opacity);
  glGenBuffers(1, &shader->buffer.color[0]);
  glGenBuffers(1, &shader->buffer.color[1]);
  glGenBuffers(1, &shader->buffer.transform);
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

    // vertex
    glBindBuffer(GL_ARRAY_BUFFER,shader->buffer.vertex);
    glEnableVertexAttribArray(shader->attribute.vertex);
    glVertexAttribPointer(shader->attribute.vertex,2,GL_FLOAT,GL_FALSE,0,NULL);

    // texture_uv
    glBindBuffer(GL_ARRAY_BUFFER,shader->buffer.uv);
    glEnableVertexAttribArray(shader->attribute.texture_uv);
    glVertexAttribPointer(shader->attribute.texture_uv, 4, GL_FLOAT, GL_FALSE, 0, NULL);

    // texture_index
    glBindBuffer(GL_ARRAY_BUFFER,shader->buffer.texture_index);
    glEnableVertexAttribArray(shader->attribute.texture_index );
    glVertexAttribIPointer(shader->attribute.texture_index,1,GL_BYTE,0,NULL);

    // opacity
    glBindBuffer(GL_ARRAY_BUFFER,shader->buffer.opacity);
    glEnableVertexAttribArray(shader->attribute.opacity);
    glVertexAttribPointer(shader->attribute.opacity,1,GL_FLOAT,GL_FALSE,0,NULL);

    // color
    for(int i=0; i<2; i++){
      glBindBuffer(GL_ARRAY_BUFFER,shader->buffer.color[i]);
      glEnableVertexAttribArray(shader->attribute.color[i]);
      glVertexAttribPointer(shader->attribute.color[i],4,GL_FLOAT,GL_FALSE,0,NULL);
    }

    // transform
    for(int i=0;i<4;i++){
      glBindBuffer(GL_ARRAY_BUFFER,shader->buffer.transform);
      glEnableVertexAttribArray(shader->attribute.transform+i );
      GLsizei stride = sizeof(GLfloat)*4*4;
      void* position = NULL+sizeof(GLfloat)*4*i;
      glVertexAttribPointer(shader->attribute.transform+i,4,GL_FLOAT,GL_FALSE,stride,position);
    }

    // extra data (node)
    for(int i=0;i<4;i++){
      glBindBuffer(GL_ARRAY_BUFFER,shader->buffer.node_extra_data);
      glEnableVertexAttribArray(shader->attribute.node_extra_data+i );
      GLsizei stride = sizeof(GLfloat)*4*4;
      void* position = NULL+sizeof(GLfloat)*4*i;
      glVertexAttribPointer(shader->attribute.node_extra_data+i,4,GL_FLOAT,GL_FALSE,stride,position);
    }

    // for instancing
    glVertexAttribDivisor(shader->attribute.vertex, 0);
    glVertexAttribDivisor(shader->attribute.texture_uv, 1);
    glVertexAttribDivisor(shader->attribute.texture_index, 1);
    glVertexAttribDivisor(shader->attribute.opacity, 1);
    glVertexAttribDivisor(shader->attribute.color[0], 1);
    glVertexAttribDivisor(shader->attribute.color[1], 1);
    for(int i=0;i<4;i++){
      glVertexAttribDivisor(shader->attribute.transform+i, 1);
      glVertexAttribDivisor(shader->attribute.node_extra_data+i, 1);
    }

  // unbind vao
  glBindVertexArray(0);
}

void bi_shader_set_uniforms(BiShader* shader,double time,int w,int h,float scale,float* attributes)
{
  glUniform1f( shader->uniform.time, time);
  glUniform2f( shader->uniform.resolution, w, h );
  glUniform1f( shader->uniform.scale, scale );
  glUniformMatrix4fv(shader->uniform.layer_extra_data, 1, GL_FALSE, attributes );
}

void bi_shader_draw(BiShader* shader,Array* queue)
{
  const size_t len = queue->size;
  static size_t len_max = 0;
  static GLfloat *transforms = NULL;
  static GLfloat *uv = NULL; // [ left, top, right, bottom ]
  static char *texture_index = NULL;
  static GLfloat* opacity = NULL;
  static GLfloat* color[2] = {NULL};
  static GLfloat *extra_data = NULL;

  if(len>len_max){
    len_max = len;
    transforms = realloc( transforms, sizeof(GLfloat)*len*16 );
    uv = realloc( uv, sizeof(GLfloat)*len*4 );
    texture_index = realloc( texture_index, sizeof(char)*len );
    opacity = realloc( opacity, sizeof(GLfloat)*len );
    color[0] = realloc( color[0], sizeof(GLfloat)*4*len );
    color[1] = realloc( color[1], sizeof(GLfloat)*4*len );
    extra_data = realloc( extra_data, sizeof(GLfloat)*len*16 );
  }

  for( uint64_t i=0; i<len; i++ ) {
    BiNode* node = queue->objects[i];
    // Matrix
    bi_mat4_copy(&transforms[i*16], node->draw_matrix);
    // Texture
    if(node->texture != NULL) {
      if(node->texture_flip_horizontal) {
        uv[i*4+0] = node->texture_uv_right; // Left <-> Right
        uv[i*4+2] = node->texture_uv_left;
      }else{
        uv[i*4+0] = node->texture_uv_left;
        uv[i*4+2] = node->texture_uv_right;
      }
      if( node->texture_flip_vertical == false ) { // XXX: DEFAULT flipped
        uv[i*4+1] = node->texture_uv_bottom; // Top <-> Bottom
        uv[i*4+3] = node->texture_uv_top;
      }else{
        uv[i*4+1] = node->texture_uv_top;
        uv[i*4+3] = node->texture_uv_bottom;
      }
      texture_index[i] = node->texture->texture_unit;
    }else{
      texture_index[i] = -1; // no-texture
    }
    // opacity, color, color2
    opacity[i] = node->opacity;
    color[0][i*4+0] = node->color[0];
    color[0][i*4+1] = node->color[1];
    color[0][i*4+2] = node->color[2];
    color[0][i*4+3] = node->color[3];
    color[1][i*4+0] = node->color2[0];
    color[1][i*4+1] = node->color2[1];
    color[1][i*4+2] = node->color2[2];
    color[1][i*4+3] = node->color2[3];
    // extra_data
    bi_mat4_copy(&extra_data[i*16], node->shader_extra_data);
  }

  //
  // update vbo
  // orphaning: https://www.khronos.org/opengl/wiki/Buffer_Object_Streaming#Buffer_re-specification
  //
  // transform
  glBindBuffer(GL_ARRAY_BUFFER, shader->buffer.transform);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 16 * len, NULL, GL_DYNAMIC_DRAW);
  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(GLfloat) * 16 * len, transforms);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  // texture_index
  glBindBuffer(GL_ARRAY_BUFFER, shader->buffer.texture_index);
  glBufferData(GL_ARRAY_BUFFER, sizeof(char) * 1 * len, NULL, GL_DYNAMIC_DRAW);
  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(char) * 1 * len, texture_index);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  // uv
  glBindBuffer(GL_ARRAY_BUFFER, shader->buffer.uv);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 4 * len, NULL, GL_DYNAMIC_DRAW);
  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(GLfloat) * 4 * len, uv);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  // opacity
  glBindBuffer(GL_ARRAY_BUFFER, shader->buffer.opacity);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * len, NULL, GL_DYNAMIC_DRAW);
  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(GLfloat) * len, opacity);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  // color
  for(int i=0;i<2;i++){
    glBindBuffer(GL_ARRAY_BUFFER, shader->buffer.color[i]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 4 * len, NULL, GL_DYNAMIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(GLfloat) * 4 * len, color[i]);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
  }
  // extra_data
  glBindBuffer(GL_ARRAY_BUFFER, shader->buffer.node_extra_data);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 16 * len, NULL, GL_DYNAMIC_DRAW);
  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(GLfloat) * 16 * len, extra_data);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  //
  // Draw instances
  //
  glBindVertexArray(shader->vao);
    glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, len);
  glBindVertexArray(0);
}
