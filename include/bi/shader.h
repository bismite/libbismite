#ifndef __BISMITE_SHADER_H__
#define __BISMITE_SHADER_H__

#include <bi/bi_gl.h>
#include <bi/layer.h>

typedef struct _BiShader BiShader;
struct _BiShader {
  GLuint program_id;
  GLuint vao;
  GLuint vertex_buffer;
  GLuint uv_buffer;
  GLuint opacity_buffer;
  GLuint texture_z_buffer;
  GLuint tint_color_buffer;
  GLuint transform_buffer;

  // uniform location
  GLint texture_locations[BI_LAYER_MAX_TEXTURES];
  GLint camera_location;

  // attribute location
  GLint vertex_location;
  GLint texture_uv_location;
  GLint opacity_location;
  GLint texture_z_location;
  GLint tint_color_location;
  GLint transform_locations[4];

  //
  GLint time_location;
  GLint resolution_location;
  GLint scale_location;
  GLint optional_attributes_location;
};

extern void bi_shader_init(BiShader* shader,const char* vertex_shader_source, const char* fragment_shader_source);
extern void bi_shader_set_uniforms(BiShader* shader,double time,int w,int h,float scale,float* attributes);
extern void bi_shader_draw(BiShader* shader,Array* queue);

#endif
