#ifndef __BI_CORE_CONTEXT_H__
#define __BI_CORE_CONTEXT_H__

#include <stdbool.h>
#include <bi/bi_gl.h>
#include <bi/bi_sdl.h>
#include <bi/shader.h>
#include <bi/profile.h>
#include <bi/node.h>
#include <bi/layer.h>
#include <bi/array.h>
#include <bi/framebuffer.h>
#include <bi/color.h>

typedef struct _BiContext BiContext;

typedef void (*on_mainloop_end_callback)(BiContext*);

struct _BiContext {
  bool running;

  // geometry
  int w;
  int h;
  bool highdpi;

  // background color
  BiColor color;

  //
  int64_t program_start_at;

  // fps, etc...
  BiProfile profile;

  // show debug info
  bool debug;

  // Layers
  BiLayerGroup layers;

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
  SDL_Window *window;

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

// Convenient Layer functions
extern void bi_add_layer(BiContext* context, BiLayer* layer);
extern void bi_update_layer_order(BiContext* context);
extern void bi_remove_layer(BiContext* context, BiLayer* layer);

//
extern const char* bi_default_vertex_shader();
extern const char* bi_default_fragment_shader();

#endif
