#include <bi/context.h>
#include <bi/bi_gl.h>
#include <bi/render.h>
#include <bi/util.h>
#include <bi/timer.h>
#include <bi/shader_node.h>
#include <bi/logger.h>
#include <bi/image.h>
#include <stdlib.h>

extern const char *SHADER_DEFAULT_VERT;
extern const char *SHADER_DEFAULT_FRAG;
extern void enable_gl_extensions();

BiContext* bi_init_context(BiContext* context,int w,int h,int fps, uint32_t flags, const char* title)
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

  uint32_t sdl_flags = SDL_WINDOW_OPENGL;
  if(flags & BI_WINDOW_ALLOW_HIGHDPI)  sdl_flags |= SDL_WINDOW_ALLOW_HIGHDPI;
  if(flags & BI_WINDOW_RESIZABLE)  sdl_flags |= SDL_WINDOW_RESIZABLE;
  context->_window = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, w, h, sdl_flags);
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

  // Default Framebuffer Node
  context->default_framebuffer.framebuffer_id = 0;
  context->default_framebuffer.texture_num = 0;
  context->default_framebuffer.w = w;
  context->default_framebuffer.h = h;
  SDL_GetWindowSizeInPixels(context->_window,
    &context->default_framebuffer.viewport_w,
    &context->default_framebuffer.viewport_h );
  bi_node_init(&context->default_framebuffer_node);
  context->default_framebuffer_node.framebuffer = &context->default_framebuffer;

  // default shader
  bi_shader_init(&context->default_shader, SHADER_DEFAULT_VERT, SHADER_DEFAULT_FRAG);

  //
  bi_profile_init(&context->profile,fps,bi_get_now());

  // callback
  context->on_mainloop_end = NULL;
  for(int i=0;i<32;i++) context->window_event_callback[i] = NULL;

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
  BiRenderingContext rendering_context;
  int64_t time = (context->program_start_at - context->frame_start_at);
  // NULL interaction_queue and timer_queue
  bi_rendering_context_init(&rendering_context,time,&context->default_shader,NULL,NULL);
  Array rendering_queue;
  array_init(&rendering_queue);
  rendering_context._rendering_queue = &rendering_queue;
  bi_render_node( rendering_context, n );
  // Clean Queue
  array_clear(&rendering_queue);
}

void bi_take_screenshot(BiContext* context, const char* filename)
{
  int w = context->default_framebuffer.viewport_w;
  int h = context->default_framebuffer.viewport_h;
  int pitch = w*4;
  uint8_t* raw = malloc(4*w*h);
  uint8_t* flipped = malloc(4*w*h);
  glBindFramebuffer(GL_FRAMEBUFFER,0);
  glReadPixels(0,0,w,h,GL_RGBA,GL_UNSIGNED_BYTE,raw);
  bi_image_rgba32_flip_vertical(w,h,raw,flipped);
  SDL_Surface* s = SDL_CreateRGBSurfaceWithFormatFrom(flipped,w,h,32,pitch,SDL_PIXELFORMAT_RGBA32);
  IMG_SavePNG(s,filename);
  SDL_FreeSurface(s);
  free(raw);
  free(flipped);
}
