#ifndef __BISMITE_NODE_BASE_H__
#define __BISMITE_NODE_BASE_H__

#include <stdbool.h>
#include <bi/timer.h>

typedef struct _BiNodeBase BiNodeBase;

typedef enum {
  BI_NODE_TYPE_NODE,
  BI_NODE_TYPE_LAYER,
  BI_NODE_TYPE_LAYER_GROUP
} BiNodeType;

#define BI_NODE_HEADER BiNodeType type; \
  int z; \
  int index; \
  bool interaction_enabled; \
  double time_scale; \
  BiTimers timers; \
  void* userdata;

struct _BiNodeBase{
  BI_NODE_HEADER
};

extern void bi_node_base_init(BiNodeBase* node,BiNodeType type);

#endif
