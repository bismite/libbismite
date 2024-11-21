#ifndef __BI_CORE_CONTEXT_H__
#define __BI_CORE_CONTEXT_H__

#include <stdbool.h>
#include <bi/array.h>
#include <bi/color.h>
#include <bi/bi_gl.h>
#include <bi/bi_sdl.h>
#include <bi/profile.h>
#include <bi/framebuffer.h>
#include <bi/shader.h>
#include <bi/shader_node.h>
#include <bi/node.h>

typedef struct _BiContext BiContext;

typedef void (*mainloop_end_callback)(BiContext*);
typedef void (*window_event_callback)(BiContext*);

typedef enum BiWindowFlags {
  BI_WINDOW_ALLOW_HIGHDPI = 1,
  BI_WINDOW_RESIZABLE
} BiWindowFlags;

struct _BiContext {
  bool running;

  // geometry
  int w;
  int h;

  // fps, etc...
  BiProfile profile;

  // show debug info
  bool debug;

  // Shader and Framebuffer
  BiFramebuffer default_framebuffer;
  BiNode default_framebuffer_node;
  BiShader default_shader;

  // queue
  Array interaction_queue;
  Array timer_queue;

  // timers
  int64_t program_start_at;
  int64_t frame_start_at;
  int64_t last_update;
  int max_delta;

  //
  SDL_Window *_window;
  SDL_GLContext *_glcontext;

  // callback
  mainloop_end_callback on_mainloop_end;
  window_event_callback window_event_callback[32];

  //
  void* userdata;
};

typedef void (*InitializeFunction)(BiContext*);

extern BiContext* bi_init_context(BiContext* context,int w, int h, int fps, uint32_t flags, const char* title );
extern void bi_set_title(BiContext* context,const char* title);
extern bool is_high_dpi(BiContext* context);

//
extern const char* bi_default_vertex_shader();
extern const char* bi_default_fragment_shader();

//
extern void bi_draw_framebuffer_node(BiContext* context, BiNode* n);

#endif
