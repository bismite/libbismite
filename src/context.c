#include <bi/context.h>
#include <bi/util.h>
#include <bi/shader.h>
#include <bi/profile.h>
#include <bi/timer.h>
#include <bi/layer.h>
#include <bi/bi_sdl.h>
#include <bi/logger.h>
#include <stdlib.h>

#ifndef __EMSCRIPTEN__
static inline void glGenVertexArrays_APPLE_wrapper(GLsizei s, GLuint *p) { glGenVertexArraysAPPLE(s,p); }
static inline void glBindVertexArray_APPLE_wrapper(GLuint vao) {glBindVertexArrayAPPLE(vao); }
#endif

static void enable_gl_extensions()
{
    // https://www.khronos.org/registry/OpenGL/extensions/ARB/ARB_instanced_arrays.txt
    if(GLEW_ARB_instanced_arrays) LOG("ARB_instanced_arrays ok\n");

    // https://www.khronos.org/registry/OpenGL/extensions/ARB/ARB_draw_instanced.txt
    if(GLEW_ARB_draw_instanced) LOG("ARB_draw_instanced ok\n");

    // https://www.khronos.org/registry/OpenGL/extensions/ARB/ARB_vertex_array_object.txt
    if(GLEW_ARB_vertex_array_object) LOG("ARB_vertex_array_object ok\n");

#ifndef __EMSCRIPTEN__
    // https://www.khronos.org/registry/OpenGL/extensions/APPLE/APPLE_vertex_array_object.txt
    if(GLEW_APPLE_vertex_array_object){
        LOG("APPLE_vertex_array_object ok\n");
        glGenVertexArrays = glGenVertexArrays_APPLE_wrapper;
        glBindVertexArray = glBindVertexArray_APPLE_wrapper;
    }
#endif

    // https://www.khronos.org/registry/webgl/extensions/ANGLE_instanced_arrays/
    if(GLEW_ANGLE_instanced_arrays) LOG("ANGLE_instanced_arrays ok\n");

#ifdef __EMSCRIPTEN__
    // https://www.khronos.org/registry/OpenGL/extensions/OES/OES_vertex_array_object.txt
    if(GLEW_OES_vertex_array_object) LOG("OES_vertex_array_object ok\n");
#endif

    // https://www.khronos.org/registry/OpenGL/extensions/EXT/EXT_draw_instanced.txt
    if(GLEW_EXT_draw_instanced) LOG("EXT_draw_instanced ok\n");
}

void bi_init_context(BiContext* context,int w,int h,int fps, bool highdpi, const char* title)
{
    context->rendering_nodes_queue_size = 0;
    context->callback_planned_nodes_size = 0;
    context->on_update_callbacks_size = 0;
    context->layers_size = 0;

    //
    double now = bi_get_now();
    bi_profile_init(&context->profile,fps,now);

    context->debug = false;

    context->running = true;

    context->w = w;
    context->h = h;
    context->camera_x = 0;
    context->camera_y = 0;

    context->color[0] = 0;
    context->color[1] = 0;
    context->color[2] = 0;
    context->color[3] = 0;

    if( SDL_Init(SDL_INIT_VIDEO) != 0 ){
        LOG("SDL_Init fail.");
        return;
    }

    SDL_SetHint(SDL_HINT_RENDER_DRIVER,"opengl");
#ifdef __EMSCRIPTEN__
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#elif __APPLE__
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
#elif __MINGW32__
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
#else
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
#endif

    Uint32 flag = SDL_WINDOW_OPENGL;
    if(highdpi == true) { flag = flag | SDL_WINDOW_ALLOW_HIGHDPI; }
    context->window = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, w, h, flag);

    // SDL_GLContext sdl_gl_context = SDL_GL_CreateContext(context->window);
    SDL_GL_CreateContext(context->window);

    if( glewInit() == GLEW_OK ){
      LOG("glewInit ok\n");
    }else{
      LOG("glewInit faild\n");
    }

    enable_gl_extensions();

    bi_init_shader(&context->shader,context->w,context->h);
}

void bi_set_title(BiContext* context, const char* title)
{
  SDL_SetWindowTitle( context->window, title );
}

//
// Layer
//

void bi_add_layer(BiContext* context, BiLayer* layer)
{
  context->layers[context->layers_size] = layer;
  context->layers_size += 1;
}

void bi_remove_layer(BiContext* context, BiLayer* layer)
{
  int index = -1;
  for(int i=0;i<context->layers_size;i++){
    if( context->layers[i] == layer ) {
      index = i;
      break;
    }
  }
  for(int i=index+1;i<context->layers_size;i++) {
    context->layers[i-1] = context->layers[i];
  }
  context->layers_size -= 1;
}
