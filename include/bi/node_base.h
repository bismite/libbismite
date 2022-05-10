#ifndef __BISMITE_NODE_BASE_H__
#define __BISMITE_NODE_BASE_H__

#include <stdbool.h>
#include <bi/timer.h>

typedef struct _BiRawNode BiRawNode;
typedef struct _BiTimerManager BiTimerManager;

typedef enum {
  BI_NODE_TYPE_NODE,
  BI_NODE_TYPE_LAYER,
  BI_NODE_TYPE_LAYER_GROUP
} BiNodeType;

#define BI_NODE_HEADER BiNodeType type; \
  int z; \
  int index; \
  bool interaction_enabled; \
  struct { \
    int size; \
    BiTimer **timers; \
    double scale; \
  } timers; \
  double time_scale; \
  void* userdata;

struct _BiRawNode{
  BI_NODE_HEADER
};

extern void bi_raw_node_init(BiRawNode* node,BiNodeType type);
extern void bi_raw_node_add_timer(BiRawNode* node,BiTimer* timer);
extern void bi_raw_node_remove_timer(BiRawNode* node,BiTimer* timer);
extern void bi_node_run_timers(BiContext* context,BiRawNode* node,double delta_time);

#endif
