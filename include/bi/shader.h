#ifndef __BISMITE_SHADER_H__
#define __BISMITE_SHADER_H__

#include <bi/bi_gl.h>
#include <bi/layer.h>

typedef struct _BiShader BiShader;
struct _BiShader {
  GLuint program_id;
  GLuint vao;

  struct {
    GLuint vertex;
    GLuint uv;
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
    GLint uv;
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
    GLint scale;
    GLint layer_extra_data;
  } uniform;
};

extern void bi_shader_init(BiShader* shader,const char* vertex_shader_source, const char* fragment_shader_source);
extern void bi_shader_set_uniforms(BiShader* shader,double time,int w,int h,float scale,float extra_data[16]);
extern void bi_shader_draw(BiShader* shader,Array* queue);

#endif
