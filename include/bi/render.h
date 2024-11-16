#ifndef __BI_RENDER_H__
#define __BI_RENDER_H__

#include <stdbool.h>
#include <bi/array.h>

typedef struct _BiNode BiNode;
typedef struct _BiContext BiContext;

typedef struct _BiRenderingContext{
  bool visible;
  bool interaction_enabled;
  double time_scale;
  Array* interaction_queue;
  Array* timer_queue;
  Array* _rendering_queue;
  BiContext* bi_context;
} BiRenderingContext;

extern void bi_render_node(BiRenderingContext,BiNode*);
extern BiRenderingContext* bi_rendering_context_init(BiRenderingContext* context,
                                                     BiContext* bi_context,
                                                     bool visible,
                                                     bool interaction_enabled,
                                                     double time_scale,
                                                     Array* interaction_queue,
                                                     Array* timer_queue );

#endif // __BI_RENDER_H__
