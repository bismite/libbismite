#ifndef __BI_EXT_TRANSITION_H__
#define __BI_EXT_TRANSITION_H__

#include <stdint.h>
#include <bi/layer.h>
#include <bi/timer.h>

typedef struct _BiContext BiContext;
typedef struct _BiShader BiShader;
typedef struct _BiTransition BiTransition;

typedef void (*bi_transition_callback)(BiTransition*);

struct _BiTransition {
  BiContext* context;
  BiLayerGroup *layer_group;
  BiLayer layer;
  BiShader* shader;
  double duration;
  bi_transition_callback callback;
  BiTimer timer;
  bool invert;
  int64_t start_at;
  bool done;
  void* userdata;
  int delay_count;
  double progress;
};

extern void bi_transition_init(BiTransition *transition,
                               BiLayerGroup *layer_group,
                               double duration,
                               bi_transition_callback callback,
                               BiShader* shader,
                               bool invert
                              );

extern void bi_transition_start(BiContext* context, BiTransition* transition);

#endif
