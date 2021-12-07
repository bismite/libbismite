#ifndef __BISMITE_RENDER_H__
#define __BISMITE_RENDER_H__

#include <stdbool.h>
#include <bi/array.h>
#include <bi/bi_gl.h>

typedef struct _BiContext BiContext;
typedef struct _BiNode BiNode;
typedef struct _BiTexture BiTexture;

extern void bi_render(BiContext* context);
extern void bi_render_node_sort(BiNode* node);

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
extern void bi_render_queuing(BiRenderingContext context, BiNode* n);
extern void bi_render_activate_textures(GLuint default_texture,BiTexture** textures);

#endif
