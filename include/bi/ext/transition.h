#ifndef __BISMITE_EXT_TRANSITION_H__
#define __BISMITE_EXT_TRANSITION_H__

#include <stdint.h>
#include <bi/layer.h>
#include <bi/texture.h>
#include <bi/timer.h>
#include <bi/canvas.h>
#include <bi/node.h>

typedef struct {
  BiLayer layer;
  BiLayerGroup *group0;
  BiLayerGroup *group1;
  BiTexture group1_framebuffer_texture;
  BiCanvas snapshot_canvas;
  BiTexture snapshot_texture;
  BiTimer timer;
} BiTransitionLayer;

BiTransitionLayer* bi_transition_layer_init(BiTransitionLayer *transition_layer,
                                     int w, int h,
                                     BiShader* postprocess_shader,
                                     BiLayerGroup* group0,
                                     BiLayerGroup* group1,
                                     timer_callback callback
                                   );

#endif
