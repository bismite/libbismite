#include <bi/shader.h>
#include <bi/camera.h>
#include <bi/util.h>
#include <bi/bi_sdl.h>
#include <bi/texture.h>
#include <bi/node.h>
#include "matrix.h"
#include <stdbool.h>
#include <stdio.h>

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
  shader->vertex_location = glGetAttribLocation(program_id, "vertex");
  shader->texture_uv_location = glGetAttribLocation(program_id, "texture_uv");
  shader->vertex_index_location = glGetAttribLocation(program_id, "vertex_index");
  shader->opacity_location = glGetAttribLocation(program_id, "opacity");
  shader->texture_z_location = glGetAttribLocation(program_id, "texture_z");
  shader->tint_color_location = glGetAttribLocation(program_id, "tint_color");
  shader->transform_locations[0] = glGetAttribLocation(program_id, "transform_a");
  shader->transform_locations[1] = glGetAttribLocation(program_id, "transform_b");
  shader->transform_locations[2] = glGetAttribLocation(program_id, "transform_c");
  shader->transform_locations[3] = glGetAttribLocation(program_id, "transform_d");

  // Uniform locations
  shader->camera_location = glGetUniformLocation(program_id, "camera");
  shader->texture_locations[0] = glGetUniformLocation(program_id, "sampler[0]");
  shader->time_location = glGetUniformLocation(program_id, "time");
  shader->resolution_location = glGetUniformLocation(program_id, "resolution");
  shader->scale_location = glGetUniformLocation(program_id, "scale");
  shader->optional_attributes_location = glGetUniformLocation(program_id, "optional_attributes");
}

void bi_shader_init(BiShader* shader,const char* vertex_shader_source, const char* fragment_shader_source)
{
  load_shader(shader,vertex_shader_source,fragment_shader_source);

  // set unit to sampler2D
  glUseProgram(shader->program_id);
  int texture_location_indexes[BI_LAYER_MAX_TEXTURES];
  for(int i=0; i<BI_LAYER_MAX_TEXTURES; i++) { texture_location_indexes[i] = i; }
  glUniform1iv(shader->texture_locations[0], BI_LAYER_MAX_TEXTURES, texture_location_indexes);
  glUseProgram(0);

  // create vbo
  glGenBuffers(1, &shader->uv_buffer);
  glGenBuffers(1, &shader->opacity_buffer);
  glGenBuffers(1, &shader->texture_z_buffer);
  glGenBuffers(1, &shader->tint_color_buffer);
  glGenBuffers(1, &shader->transform_buffer);

  // create vbo : vertex
  glGenBuffers(1, &shader->vertex_buffer);
  glBindBuffer(GL_ARRAY_BUFFER, shader->vertex_buffer);
  GLfloat l=0, t=1, r=1, b=0; // vertex left,top,right,bottom
  GLfloat vertex[8] = { l,t, l,b, r,t, r,b };
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 8, vertex, GL_DYNAMIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  // create vbo : vertex index
  glGenBuffers(1, &shader->vertex_index_buffer);
  glBindBuffer(GL_ARRAY_BUFFER, shader->vertex_index_buffer);
  GLfloat vertex_index[4] = {0,1,2,3}; // XXX: prior OpenGL3 and ES3, can not send int...
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 4, vertex_index, GL_DYNAMIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  // generate vao
  glGenVertexArrays(1, &shader->vao);
  glBindVertexArray(shader->vao);

    // vertex
    glBindBuffer(GL_ARRAY_BUFFER,shader->vertex_buffer);
    glEnableVertexAttribArray(shader->vertex_location);
    glVertexAttribPointer(shader->vertex_location,2,GL_FLOAT,GL_FALSE,0,NULL);

    // texture_uv
    glBindBuffer(GL_ARRAY_BUFFER,shader->uv_buffer);
    glEnableVertexAttribArray(shader->texture_uv_location);
    glVertexAttribPointer(shader->texture_uv_location, 4, GL_FLOAT, GL_FALSE, 0, NULL);

    // vertex_index (XXX: glVertexAttribIPointer is not available in WebGL 1.0)
    glBindBuffer(GL_ARRAY_BUFFER,shader->vertex_index_buffer);
    glEnableVertexAttribArray(shader->vertex_index_location);
    glVertexAttribPointer(shader->vertex_index_location,1,GL_FLOAT,GL_FALSE,0,NULL);

    // opacity
    glBindBuffer(GL_ARRAY_BUFFER,shader->opacity_buffer);
    glEnableVertexAttribArray(shader->opacity_location );
    glVertexAttribPointer(shader->opacity_location,1,GL_FLOAT,GL_FALSE,0,NULL);

    // texture_z (XXX: glVertexAttribIPointer is not available in WebGL 1.0)
    glBindBuffer(GL_ARRAY_BUFFER,shader->texture_z_buffer);
    glEnableVertexAttribArray(shader->texture_z_location );
    glVertexAttribPointer(shader->texture_z_location,1,GL_FLOAT,GL_FALSE,0,NULL);

    // tint color
    glBindBuffer(GL_ARRAY_BUFFER,shader->tint_color_buffer);
    glEnableVertexAttribArray(shader->tint_color_location);
    glVertexAttribPointer(shader->tint_color_location,4,GL_FLOAT,GL_FALSE,0,NULL);

    // transform
    for(int i=0;i<4;i++){
      glBindBuffer(GL_ARRAY_BUFFER,shader->transform_buffer);
      glEnableVertexAttribArray(shader->transform_locations[i] );
      GLsizei stride = sizeof(GLfloat)*4*4;
      void* position = NULL+sizeof(GLfloat)*4*i;
      glVertexAttribPointer(shader->transform_locations[i],4,GL_FLOAT,GL_FALSE,stride,position);
    }

    // for instancing
    glVertexAttribDivisor(shader->vertex_location, 0);
    glVertexAttribDivisor(shader->texture_uv_location, 1);
    glVertexAttribDivisor(shader->vertex_index_location, 0);
    glVertexAttribDivisor(shader->opacity_location, 1);
    glVertexAttribDivisor(shader->texture_z_location, 1);
    glVertexAttribDivisor(shader->tint_color_location, 1);
    for(int i=0;i<4;i++){
      glVertexAttribDivisor(shader->transform_locations[i], 1);
    }

  // unbind vao
  glBindVertexArray(0);
}

void bi_shader_set_uniforms(BiShader* shader,double time,int w,int h,float scale,float* attributes)
{
  glUniform1f(shader->time_location, time);
  glUniform2f(shader->resolution_location, w, h );
  glUniform1f(shader->scale_location, scale );
  glUniform4fv(shader->optional_attributes_location, 1, attributes );
}

void bi_shader_draw(BiShader* shader,Array* queue)
{
  const size_t len = queue->size;
  static size_t len_max = 0;
  static GLfloat *transforms = NULL;
  static GLfloat *uv = NULL; // [ left, top, right, bottom ]
  static GLfloat *texture_z = NULL;
  static GLfloat *tint = NULL;
  static GLfloat *opacity = NULL;
  if(len>len_max){
    len_max = len;
    transforms = realloc( transforms, sizeof(GLfloat)*len*16 );
    uv = realloc( uv, sizeof(GLfloat)*len*4 );
    texture_z = realloc( texture_z, sizeof(GLfloat)*len );
    tint = realloc( tint, sizeof(GLfloat)*len*4 );
    opacity = realloc( opacity, sizeof(GLfloat)*len );
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
      texture_z[i] = node->texture->texture_unit;
    }else{
      texture_z[i] = -1; // no-texture
    }
    // color
    tint[i*4+0] = node->color[0] / 255.0;
    tint[i*4+1] = node->color[1] / 255.0;
    tint[i*4+2] = node->color[2] / 255.0;
    tint[i*4+3] = node->color[3] / 255.0;
    opacity[i] = node->opacity;
  }

  //
  // update vbo
  // orphaning: https://www.khronos.org/opengl/wiki/Buffer_Object_Streaming#Buffer_re-specification
  //
  // transform
  glBindBuffer(GL_ARRAY_BUFFER, shader->transform_buffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 16 * len, NULL, GL_DYNAMIC_DRAW);
  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(GLfloat) * 16 * len, transforms);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  // texture_z (texture index send as float. not integer.)
  glBindBuffer(GL_ARRAY_BUFFER, shader->texture_z_buffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 1 * len, NULL, GL_DYNAMIC_DRAW);
  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(GLfloat) * 1 * len, texture_z);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  // uv
  glBindBuffer(GL_ARRAY_BUFFER, shader->uv_buffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 4 * len, NULL, GL_DYNAMIC_DRAW);
  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(GLfloat) * 4 * len, uv);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  // tint color
  glBindBuffer(GL_ARRAY_BUFFER, shader->tint_color_buffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 4 * len, NULL, GL_DYNAMIC_DRAW);
  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(GLfloat) * 4 * len, tint);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  // opacity
  glBindBuffer(GL_ARRAY_BUFFER, shader->opacity_buffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 1 * len, NULL, GL_DYNAMIC_DRAW);
  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(GLfloat) * 1 * len, opacity);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  //
  // Draw instances
  //
  glBindVertexArray(shader->vao);
    glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, len);
  glBindVertexArray(0);
}
