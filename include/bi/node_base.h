#ifndef __BISMITE_NODE_BASE_H__
#define __BISMITE_NODE_BASE_H__

#include <stdbool.h>
#include <bi/timer.h>
#include <bi/array.h>

typedef struct _BiNodeBase BiNodeBase;

typedef enum {
  BI_NODE,
  BI_LAYER,
  BI_LAYER_GROUP
} BiNodeClass;

#define BI_NODE_HEADER \
  BiNodeClass class; \
  BiNodeBase *parent; \
  Array children; \
  int z; \
  int index; \
  bool interaction_enabled; \
  double time_scale; \
  BiTimers timers; \
  void* userdata;

struct _BiNodeBase{
  BI_NODE_HEADER
};

extern void bi_node_base_init(BiNodeBase* node,BiNodeClass class);

#endif
