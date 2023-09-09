#ifndef __BISMITE_NODE_BASE_H__
#define __BISMITE_NODE_BASE_H__

#include <stdbool.h>
#include <bi/timer.h>

typedef struct _BiNodeBase BiNodeBase;

#define BI_NODE_HEADER \
  BiNodeBase *parent; \
  int z; \
  int index; \
  bool interaction_enabled; \
  double time_scale; \
  BiTimers timers; \
  void* userdata;

struct _BiNodeBase{
  BI_NODE_HEADER
};

extern void bi_node_base_init(BiNodeBase* node);

#endif
