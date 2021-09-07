#ifndef __BI_CORE_SHADER_H__
#define __BI_CORE_SHADER_H__

#include <bi/bi_gl.h>
#include <bi/layer.h>

typedef struct _BiShader BiShader;

struct _BiShader {
    GLuint program_id;
    GLuint vao;
    GLuint vertex_buffer;
    GLuint uv_buffer;
    GLuint vertex_index_buffer;
    GLuint texture_z_buffer;
    GLuint mod_color_buffer;
    GLuint transform_buffer;

    // uniform location
    GLint texture_locations[BI_LAYER_MAX_TEXTURES];
    GLint projection_location;
    GLint view_location;

    // attribute location
    GLint vertex_location;
    GLint texture_uv_location;
    GLint vertex_index_location;
    GLint texture_z_location;
    GLint mod_color_location;
    GLint transform_locations[4];

    //
    GLint time_location;
    GLint resolution_location;
    GLint scale_location;
    GLint optional_attributes_location;
};

extern void bi_shader_init(BiShader* shader,const char* vertex_shader_source, const char* fragment_shader_source);

#endif
