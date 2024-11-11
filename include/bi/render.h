#ifndef __BISMITE_RENDER_H__
#define __BISMITE_RENDER_H__

#include <stdbool.h>
#include <bi/array.h>

typedef struct _BiNodeBase BiNodeBase;
typedef struct _BiFramebuffer BiFramebuffer;
typedef struct _BiContext BiContext;
typedef struct _BiNode BiNode;
typedef struct _BiTexture BiTexture;

typedef struct _BiRenderingContext{
  bool visible;
  bool interaction_enabled;
  double time_scale;
  Array* interaction_queue;
  Array* timer_queue;
  Array* rendering_queue;
} BiRenderingContext;

extern BiRenderingContext* bi_rendering_context_init(BiRenderingContext* context,
                                                     bool visible,
                                                     bool interaction_enabled,
                                                     double time_scale,
                                                     Array* interaction_queue,
                                                     Array* timer_queue,
                                                     Array* rendering_queue);

extern void bi_render_layer_group(BiContext* context,
                                  BiNodeBase *layer_group,
                                  BiFramebuffer* dst,
                                  BiRenderingContext rc );

#endif
