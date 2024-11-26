#ifndef __BISMITE_SHADER_H__
#define __BISMITE_SHADER_H__

#include <bi/bi_gl.h>
#include <bi/array.h>

#define BI_SHADER_MAX_TEXTURES 16

typedef struct _BiFramebuffer BiFramebuffer;
typedef struct _BiShaderNode BiShaderNode;
typedef struct _BiShader BiShader;

struct _BiShader {
  GLuint program_id;
  GLuint vao;

  struct {
    GLuint vertex;
    GLuint texture_uv;
    GLuint texture_crop_uv;
    GLuint texture_index;
    GLuint tint;
    GLuint modulate;
    GLuint node_size;
    GLuint transform;
    GLuint node_extra_data;
  } buffer;

  // attribute locations
  struct {
    GLint vertex;
    GLint texture_uv;
    GLint texture_crop_uv;
    GLint texture_index;
    GLint tint;
    GLint modulate;
    GLint node_size;
    GLint transform;
    GLint node_extra_data;
  } attribute;

  // uniform location
  struct {
    GLint camera;
    GLint texture;
    GLint time;
    GLint resolution;
    GLint viewport_size;
    GLint scale;
    GLint shader_extra_data;
  } uniform;
};

extern void bi_shader_init(BiShader* shader,const char* vertex_shader_source, const char* fragment_shader_source);
extern void bi_shader_set_uniforms(BiShader* shader,double time,BiFramebuffer* fb,BiShaderNode* snode);
extern void bi_shader_draw(BiShader* shader,Array* queue);

#endif
