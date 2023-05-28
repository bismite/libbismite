#include <bi/context.h>
#include <bi/util.h>
#include <bi/timer.h>
#include <bi/layer.h>
#include <bi/logger.h>
#include <stdlib.h>
#include "shader/default_shader.h"

extern void enable_gl_extensions();

BiContext* bi_init_context(BiContext* context,int w,int h,int fps, bool highdpi, const char* title)
{
  if( SDL_Init(SDL_INIT_VIDEO) != 0 ){
      LOG("SDL_Init fail.");
      return NULL;
  }

  SDL_SetHint(SDL_HINT_RENDER_DRIVER,"opengl");
#if defined(__EMSCRIPTEN__)
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#else
  SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
#endif

  Uint32 flag = SDL_WINDOW_OPENGL;
  if(highdpi == true) { flag = flag | SDL_WINDOW_ALLOW_HIGHDPI; }
  context->window = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, w, h, flag);

  SDL_GLContext *glcontext = SDL_GL_CreateContext(context->window);
  if(glcontext==NULL){
    printf("SDL_GL_CreateContext failed: %s\n",SDL_GetError());
    exit(1);
  }

  enable_gl_extensions(context);
  glEnable(GL_BLEND);

  context->program_start_at = bi_get_now();

  array_init(&context->rendering_queue);
  array_init(&context->interaction_queue);
  array_init(&context->timer_queue);

  bi_layer_group_init(&context->layers);

  // timers
  context->last_update = 0;
  context->max_delta = 100;

  //
  bi_profile_init(&context->profile,fps,bi_get_now());

  context->debug = false;

  context->running = true;

  context->w = w;
  context->h = h;
  context->highdpi = highdpi;

  context->color[0] = 0;
  context->color[1] = 0;
  context->color[2] = 0;
  context->color[3] = 0;

  // default shader
  bi_shader_init(&context->default_shader, DEFAULT_VERTEX_SHADER, DEFAULT_FRAGMENT_SHADER);

  // default texture
#ifdef __APPLE__
  uint8_t pixels[4] = {0,0,0,0};
  glBindTexture(GL_TEXTURE_2D, 0);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
  glBindTexture(GL_TEXTURE_2D, 0);
#endif

  // framebuffer
  GLint dims[4] = {0};
  glGetIntegerv(GL_VIEWPORT, dims);
  bi_framebuffer_init(&context->layer_framebuffer,dims[2],dims[3]);
  bi_framebuffer_init(&context->post_process_framebuffer,dims[2],dims[3]);

  return context;
}

void bi_set_title(BiContext* context, const char* title)
{
  SDL_SetWindowTitle( context->window, title );
}


//
// Layer
//
void bi_add_layer(BiContext* context, BiLayer* layer) { bi_layer_group_add_layer(&context->layers,layer); }
void bi_update_layer_order(BiContext* context) { bi_layer_group_update_order(&context->layers); }
void bi_remove_layer(BiContext* context, BiLayer* layer) { bi_layer_group_remove_layer(&context->layers,layer); }

//
// Shader
//
const char* bi_default_vertex_shader()
{
  return DEFAULT_VERTEX_SHADER;
}

const char* bi_default_fragment_shader()
{
  return DEFAULT_FRAGMENT_SHADER;
}
