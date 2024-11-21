#include <bi/context.h>
#include <bi/bi_gl.h>
#include <bi/render.h>
#include <bi/util.h>
#include <bi/timer.h>
#include <bi/shader_node.h>
#include <bi/logger.h>
#include <stdlib.h>

extern const char *SHADER_DEFAULT_VERT;
extern const char *SHADER_DEFAULT_FRAG;
extern void enable_gl_extensions();

BiContext* bi_init_context(BiContext* context,int w,int h,int fps, bool highdpi, const char* title)
{
  context->debug = false;
  context->running = false;

  // Size
  context->w = w;
  context->h = h;

  // Timers
  context->program_start_at = bi_get_now();
  context->frame_start_at = 0;
  context->last_update = 0;
  context->max_delta = 100;

  //
  // Init
  //
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
  context->_window = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, w, h, flag);

  context->_glcontext = SDL_GL_CreateContext(context->_window);
  if(context->_glcontext==NULL){
    printf("SDL_GL_CreateContext failed: %s\n",SDL_GetError());
    return NULL;
  }

  enable_gl_extensions();
  glEnable(GL_BLEND);

  // Queue
  array_init(&context->interaction_queue);
  array_init(&context->timer_queue);

  // Framebuffer
  context->default_framebuffer.framebuffer_id = 0;
  context->default_framebuffer.texture_id = 0;
  int real_w, real_h;
  SDL_GetWindowSizeInPixels(context->_window,&real_w,&real_h);
  context->default_framebuffer.w = real_w;
  context->default_framebuffer.h = real_h;

  bi_node_init(&context->default_framebuffer_node);
  context->default_framebuffer_node.framebuffer = &context->default_framebuffer;
  // default shader
  bi_shader_init(&context->default_shader, SHADER_DEFAULT_VERT, SHADER_DEFAULT_FRAG);

  //
  bi_profile_init(&context->profile,fps,bi_get_now());

  // default texture
#ifdef __APPLE__
  uint8_t pixels[4] = {0,0,0,0};
  glBindTexture(GL_TEXTURE_2D, 0);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
  glBindTexture(GL_TEXTURE_2D, 0);
#endif

  // mainloop_end_callback
  context->on_mainloop_end = NULL;

  return context;
}

void bi_set_title(BiContext* context, const char* title)
{
  SDL_SetWindowTitle( context->_window, title );
}

bool is_high_dpi(BiContext* context)
{
  return (SDL_GetWindowFlags(context->_window) & SDL_WINDOW_ALLOW_HIGHDPI) == SDL_WINDOW_ALLOW_HIGHDPI;
}

//
// Shader
//
const char* bi_default_vertex_shader()
{
  return SHADER_DEFAULT_VERT;
}

const char* bi_default_fragment_shader()
{
  return SHADER_DEFAULT_FRAG;
}

//
// Framebuffer Draw
//
void bi_draw_framebuffer_node(BiContext* context, BiNode* n)
{
  // viewport setting
  GLint tmp[4];
  glGetIntegerv(GL_VIEWPORT,tmp);
  glViewport(0,0,n->w,n->h);
  // camera
  int _w = context->w;
  int _h = context->h;
  context->w = n->w;
  context->h = n->h;
  // rendering
  BiRenderingContext rendering_context;
  // NULL interaction_queue and timer_queue
  bi_rendering_context_init(&rendering_context,context,true,true,1.0, NULL, NULL );
  Array rendering_queue;
  array_init(&rendering_queue);
  rendering_context._rendering_queue = &rendering_queue;
  bi_render_node( rendering_context, n );
  // restore
  context->w = _w;
  context->h = _h;
  // restore viewport
  glViewport(tmp[0],tmp[1],tmp[2],tmp[3]);
  // Clean Queue
  array_clear(&rendering_queue);
}
