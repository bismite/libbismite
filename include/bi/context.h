#ifndef __BI_CORE_CONTEXT_H__
#define __BI_CORE_CONTEXT_H__

#include <stdbool.h>
#include <bi/array.h>
#include <bi/color.h>
#include <bi/bi_gl.h>
#include <bi/bi_sdl.h>
#include <bi/profile.h>
#include <bi/framebuffer.h>
#include <bi/framebuffer_node.h>
#include <bi/shader.h>
#include <bi/shader_node.h>
#include <bi/node.h>

typedef struct _BiContext BiContext;

typedef void (*on_mainloop_end_callback)(BiContext*);

struct _BiContext {
  bool running;

  // geometry
  int w;
  int h;

  //
  int64_t program_start_at;

  // fps, etc...
  BiProfile profile;

  // show debug info
  bool debug;

  // Default Framebuffer
  BiFramebuffer default_framebuffer;
  BiNode default_framebuffer_node;

  // queue
  Array rendering_queue;
  Array interaction_queue;
  Array timer_queue;

  // timers
  int64_t frame_start_at;
  int64_t last_update;
  int max_delta;

  // default shader
  BiShader default_shader;

  //
  SDL_Window *_window;
  SDL_GLContext *_glcontext;

  // for postprocess
  BiFramebuffer post_process_framebuffer;

  // callback
  on_mainloop_end_callback on_mainloop_end;

  //
  void* userdata;
};

typedef void (*InitializeFunction)(BiContext*);

extern BiContext* bi_init_context(BiContext* context,int w, int h, int fps, bool highdpi, const char* title );
extern void bi_set_title(BiContext* context,const char* title);
extern bool is_high_dpi(BiContext* context);

// Convenient Layer functions
extern void bi_add_shader_node(BiContext* context, BiShaderNode* shader_node);
extern void bi_update_shader_node_order(BiContext* context);
extern void bi_remove_shader_node(BiContext* context, BiShaderNode* shader_node);

//
extern const char* bi_default_vertex_shader();
extern const char* bi_default_fragment_shader();

#endif
