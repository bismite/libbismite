#ifndef __BI_CORE_GL_EXT_H__
#define __BI_CORE_GL_EXT_H__

//
// wrapper
//

#ifndef __EMSCRIPTEN__
static inline void glGenVertexArrays_APPLE_wrapper(GLsizei s, GLuint *p) { glGenVertexArraysAPPLE(s,p); }
static inline void glBindVertexArray_APPLE_wrapper(GLuint vao) {glBindVertexArrayAPPLE(vao); }
#endif

static inline void checkSupports()
{
    SDL_Log("OpenGL Version: %s\n", glGetString(GL_VERSION));
    SDL_Log("OpenGL Renderer: %s\n", glGetString(GL_RENDERER));
    SDL_Log("GLSL Version: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));

    // iphone7: 4096
    GLint max_texture_size;
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &max_texture_size);
    SDL_Log("GL_MAX_TEXTURE_SIZE: %d\n",max_texture_size);

    // iphone7: 0 (not supported!)
    GLint max_array_texture_layers;
    glGetIntegerv(GL_MAX_ARRAY_TEXTURE_LAYERS, &max_array_texture_layers);
    SDL_Log("GL_MAX_ARRAY_TEXTURE_LAYERS: %d\n",max_array_texture_layers);

    // iphone7: 8
    GLint max_texture_image_units;
    glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &max_texture_image_units);
    SDL_Log("GL_MAX_TEXTURE_IMAGE_UNITS: %d\n",max_texture_image_units);

    // if(GLEW_ARB_vertex_program) SDL_Log("ARB_vertex_program ok\n");

    // https://www.khronos.org/registry/OpenGL/extensions/ARB/ARB_instanced_arrays.txt
    if(GLEW_ARB_instanced_arrays) SDL_Log("ARB_instanced_arrays ok\n");

    // https://www.khronos.org/registry/OpenGL/extensions/ARB/ARB_draw_instanced.txt
    if(GLEW_ARB_draw_instanced) SDL_Log("ARB_draw_instanced ok\n");

    // https://www.khronos.org/registry/OpenGL/extensions/ARB/ARB_vertex_array_object.txt
    if(GLEW_ARB_vertex_array_object) SDL_Log("ARB_vertex_array_object ok\n");

#ifndef __EMSCRIPTEN__
    // https://www.khronos.org/registry/OpenGL/extensions/APPLE/APPLE_vertex_array_object.txt
    if(GLEW_APPLE_vertex_array_object){
        SDL_Log("APPLE_vertex_array_object ok\n");
        glGenVertexArrays = glGenVertexArrays_APPLE_wrapper;
        glBindVertexArray = glBindVertexArray_APPLE_wrapper;
    }
#endif

    // https://www.khronos.org/registry/webgl/extensions/ANGLE_instanced_arrays/
    if(GLEW_ANGLE_instanced_arrays) SDL_Log("ANGLE_instanced_arrays ok\n");

#ifdef __EMSCRIPTEN__
    // https://www.khronos.org/registry/OpenGL/extensions/OES/OES_vertex_array_object.txt
    if(GLEW_OES_vertex_array_object) SDL_Log("OES_vertex_array_object ok\n");
#endif

    // https://www.khronos.org/registry/OpenGL/extensions/EXT/EXT_instanced_arrays.txt
    // XXX: for Apple OpenGL ES?
    // if(GLEW_EXT_instanced_arrays) SDL_Log("EXT_instanced_arrays ok\n");

    // https://www.khronos.org/registry/OpenGL/extensions/EXT/EXT_draw_instanced.txt
    if(GLEW_EXT_draw_instanced) SDL_Log("EXT_draw_instanced ok\n");

    SDL_Log("checkSupports done.\n");
}

#endif
