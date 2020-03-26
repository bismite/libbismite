#ifndef __BI_CORE_SHADER_H__
#define __BI_CORE_SHADER_H__

#include <stdbool.h>
#include <bi/bi_gl.h>

struct _BiShader;
typedef struct _BiShader BiShader;

struct _BiShader {
    GLuint programId;
    GLuint vao;
    GLuint vertex_buffer;
    GLuint uv_buffer;
    GLuint vertex_index_buffer;
    GLuint texture_z_buffer;
    GLuint mod_color_buffer;
    GLuint transform_buffer;

    // uniform location
    GLint texture_locations[8];
    GLint projection_location;
    GLint view_location;

    // attribute location
    GLint vertex_location;
    GLint texture_uv_location;
    GLint vertex_index_location;
    GLint texture_z_location;
    GLint mod_color_location;
    GLint transform_locations[4];
};

extern void bi_init_shader(BiShader* shader,int w,int h);
extern void bi_set_projection(BiShader* shader,int w,int h,bool centering);
extern void bi_set_camera(BiShader* shader,int x,int y);

#endif
