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

typedef struct _BiContext BiContext;

struct _BiContext {
  bool running;

  // geometry
  int w;
  int h;

  // background color
  uint8_t color[4];

  //
  int64_t program_start_at;

  // fps, etc...
  BiProfile profile;

  // show debug info
  bool debug;

  // Layers
  BiLayerGroup layers;

  // queue
  Array _rendering_queue;
  Array _callback_queue;

  // timers
  BiTimerManager timers;
  double time_scale;
  int64_t _frame_start_at;
  int64_t _last_update;
  int max_delta;

  // default shader
  BiShader default_shader;

  //
  SDL_Window *window;

  // default texture
  GLuint default_texture;

  //
  BiFramebuffer _layer_framebuffer;
  BiFramebuffer _post_process_framebuffer;

  //
  void* userdata;
};

typedef void (*InitializeFunction)(BiContext*);

extern void bi_init_context(BiContext* context,int w, int h, int fps, bool highdpi, const char* title );
extern void bi_set_title(BiContext* context,const char* title);

// Convenient Layer functions
extern void bi_add_layer(BiContext* context, BiLayer* layer);
extern void bi_update_layer_order(BiContext* context);
extern void bi_remove_layer(BiContext* context, BiLayer* layer);

#endif
