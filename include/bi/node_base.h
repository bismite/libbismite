#ifndef __BISMITE_NODE_BASE_H__
#define __BISMITE_NODE_BASE_H__

#include <stddef.h>
#include <stdbool.h>
#include <bi/timer.h>

typedef struct _BiRawNode BiRawNode;

typedef enum {
  BI_NODE_TYPE_NODE,
  BI_NODE_TYPE_LAYER,
  BI_NODE_TYPE_LAYER_GROUP
} BiNodeType;

#define BI_NODE_HEADER BiNodeType type; \
  int z; \
  int _index; \
  bool interaction_enabled; \
  BiTimerManager timers; \
  double time_scale; \
  void* userdata;

struct _BiRawNode{
  BI_NODE_HEADER
};

inline static void bi_raw_node_init(BiRawNode* n,BiNodeType type)
{
  n->type = type;
  n->z = 0;
  n->_index = 0;
  n->interaction_enabled = true;
  // timers
  bi_timer_manager_init(&n->timers);
  n->time_scale = 1.0;
}

inline static void bi_node_add_timer(void* n, BiTimer* t)
{
  BiRawNode *_n = n;
  bi_timer_manager_add_timer(&_n->timers,t);
  t->node = _n;
}

inline static void bi_node_remove_timer(void* n, BiTimer* t)
{
  BiRawNode *_n = n;
  bi_timer_manager_remove_timer(&_n->timers,t);
  t->node = NULL;
}

#endif
