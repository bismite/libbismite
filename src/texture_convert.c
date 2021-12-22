#include <stdlib.h>
#include <stdio.h>
#include <bi/texture.h>
#include <bi/camera.h>
#include <bi/bi_gl.h>
#include "texconv_shader.h"

typedef struct {
  GLuint program_id;
  GLuint vao;
  GLint texture;
  GLint vertex;
  GLint tex_coord;
  GLuint vertex_buffer;
  GLuint tex_coord_buffer;
} TexconvShader;

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

static void load_texconv_shader(TexconvShader* shader)
{
  const char* v_shader_src = TEXCONV_VERTEX_SHADER;
  const char* f_shader_src = TEXCONV_FRAGMENT_SHADER;
  GLuint vid = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vid, 1, &v_shader_src, NULL);
  glCompileShader(vid);
  print_shader_log("Vertex Shader Log",vid);
  GLuint fid = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fid, 1, &f_shader_src, NULL);
  glCompileShader(fid);
  print_shader_log("Fragment Shader Log",fid);
  //
  shader->program_id = glCreateProgram();
  glAttachShader(shader->program_id,vid);
  glAttachShader(shader->program_id,fid);
  glLinkProgram(shader->program_id);
  // clean
  glDeleteShader(vid);
  glDeleteShader(fid);
  // locations
  shader->texture = glGetUniformLocation(shader->program_id, "texture");
  shader->vertex = glGetAttribLocation(shader->program_id, "vertex");
  shader->tex_coord = glGetAttribLocation(shader->program_id, "tex_coord");

  // vbo
  glGenBuffers(1, &shader->vertex_buffer);
  glGenBuffers(1, &shader->tex_coord_buffer);
  // vao
  glGenVertexArrays(1, &shader->vao);
  glBindVertexArray(shader->vao);
    // vertex
    glBindBuffer(GL_ARRAY_BUFFER,shader->vertex_buffer);
    glEnableVertexAttribArray(shader->vertex);
    glVertexAttribPointer(shader->vertex,2,GL_FLOAT,GL_FALSE,0,NULL);
    // texture_uv
    glBindBuffer(GL_ARRAY_BUFFER,shader->tex_coord_buffer);
    glEnableVertexAttribArray(shader->tex_coord);
    glVertexAttribPointer(shader->tex_coord,2,GL_FLOAT,GL_FALSE,0,NULL);

    glVertexAttribDivisor(shader->vertex, 0);
    glVertexAttribDivisor(shader->tex_coord, 0);
  // unbind vao
  glBindVertexArray(0);
}

static void generate_framebuffer(GLuint* fb, GLuint* tex,GLint tex_format,int w,int h)
{
  glGenFramebuffers(1, fb);
  glBindFramebuffer(GL_FRAMEBUFFER, *fb);
  glGenTextures(1, tex);
  glBindTexture(GL_TEXTURE_2D, *tex);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexImage2D(GL_TEXTURE_2D, 0, tex_format, w,h, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, *tex, 0);
  // unbind
  glBindTexture(GL_TEXTURE_2D, 0);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

GLuint bi_texture_convert_to_premultiplied_alpha(GLuint texture_id,GLint tex_format,int w,int h)
{
  static TexconvShader shader = {0};
  if(shader.program_id==0){
    load_texconv_shader(&shader);
  }

  // create framebuffer
  GLuint framebuffer_id;
  GLuint new_texture_id;
  generate_framebuffer(&framebuffer_id,&new_texture_id,tex_format,w,h);

  //
  // convert texture
  //
  glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_id);
  GLint original_viewport[4];
  glGetIntegerv(GL_VIEWPORT,original_viewport);
  glViewport(0,0,w,h);

  // alpha blending
  glBlendFuncSeparate(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA,GL_ONE,GL_ONE_MINUS_SRC_ALPHA);

  // clear Framebuffer
  glClearColor(0,0,0,0);
  glClear(GL_COLOR_BUFFER_BIT);

  // shader
  glUseProgram(shader.program_id);

  // texture
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, texture_id);
  glUniform1i(shader.texture, 0);

  // VBO vertex
  GLfloat vertex[8] = { -1,1, -1,-1, 1,1, 1,-1 }; // LeftTop, LeftBottom, RightTop, RightBottom
  glBindBuffer(GL_ARRAY_BUFFER, shader.vertex_buffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*8, NULL, GL_STATIC_DRAW);
  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(GLfloat)*8, vertex);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  // VBO texture coordinate
  GLfloat tex_coord[8] = { 0,1, 0,0, 1,1, 1,0 }; // LeftTop, LeftBottom, RightTop, RightBottom
  glBindBuffer(GL_ARRAY_BUFFER, shader.tex_coord_buffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*8, NULL, GL_STATIC_DRAW);
  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(GLfloat)*8, tex_coord);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  // draw
  glBindVertexArray(shader.vao);
    glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, 1);
  glBindVertexArray(0);

  // restore viewport
  glViewport(original_viewport[0],original_viewport[1],original_viewport[2],original_viewport[3]);
  // unbind
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  // delete
  glDeleteFramebuffers(1,&framebuffer_id);
  glDeleteTextures(1,&texture_id);
  return new_texture_id;
}
