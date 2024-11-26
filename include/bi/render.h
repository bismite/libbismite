#ifndef __BI_RENDER_H__
#define __BI_RENDER_H__

#include <stdbool.h>
#include <bi/array.h>

typedef struct _BiNode BiNode;
typedef struct _BiShader BiShader;

typedef struct _BiRenderingContext{
  bool _visible;
  bool _interaction_enabled;
  double _time_scale;
  int64_t time;
  BiShader* default_shader;
  Array* interaction_queue;
  Array* timer_queue;
  Array* _rendering_queue;
} BiRenderingContext;

extern void bi_render_node(BiRenderingContext,BiNode*);
extern BiRenderingContext* bi_rendering_context_init(BiRenderingContext* context,
                                                     int64_t time,
                                                     BiShader* default_shader,
                                                     Array* interaction_queue,
                                                     Array* timer_queue );

#endif // __BI_RENDER_H__
