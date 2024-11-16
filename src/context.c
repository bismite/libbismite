#include <bi/context.h>
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
    exit(1);
  }

  enable_gl_extensions();
  glEnable(GL_BLEND);

  context->program_start_at = bi_get_now();

  array_init(&context->rendering_queue);
  array_init(&context->interaction_queue);
  array_init(&context->timer_queue);

  // Framebuffer
  context->default_framebuffer.framebuffer_id = 0;
  context->default_framebuffer.texture_id = 0;
  context->default_framebuffer.w = w;
  context->default_framebuffer.h = h;
  if( is_high_dpi(context) ){
    context->default_framebuffer.w *= 2;
    context->default_framebuffer.h *= 2;
  }
  bi_node_init(&context->default_framebuffer_node);
  context->default_framebuffer_node.framebuffer = &context->default_framebuffer;

  // timers
  context->last_update = 0;
  context->max_delta = 100;

  //
  bi_profile_init(&context->profile,fps,bi_get_now());

  context->debug = false;

  context->running = true;

  context->w = w;
  context->h = h;

  // default shader
  bi_shader_init(&context->default_shader, SHADER_DEFAULT_VERT, SHADER_DEFAULT_FRAG);

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
  bi_framebuffer_init(&context->post_process_framebuffer,dims[2],dims[3]);

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
// Layer
//
void bi_add_shader_node(BiContext* context, BiShaderNode* s) { bi_node_add_node(&context->default_framebuffer_node,s); }
void bi_remove_shader_node(BiContext* context, BiShaderNode* s) { bi_node_add_node(&context->default_framebuffer_node,s); }

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
