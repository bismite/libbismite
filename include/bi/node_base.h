#ifndef __BI_NODE_BASE_H__
#define __BI_NODE_BASE_H__

#include <stdbool.h>
#include <bi/timer.h>
#include <bi/array.h>

typedef struct _BiContext BiContext;
typedef struct _BiNodeBase BiNodeBase;

typedef void (*on_add_child)(void*, void*); // parent,child

typedef enum {
  BI_NODE,
  BI_SHADER_NODE
} BiNodeClass;

#define BI_NODE_HEADER \
  BiNodeClass class; \
  BiNodeBase *parent; \
  Array children; \
  on_add_child _on_add_child; \
  int _z; \
  int index; \
  bool interaction_enabled; \
  double time_scale; \
  BiTimers timers; \
  Array *_interaction_queue; \
  Array *_timer_queue; \
  void* userdata;

struct _BiNodeBase{
  BI_NODE_HEADER
};

void bi_node_base_init(BiNodeBase* node,BiNodeClass class);
void bi_node_base_deinit(BiNodeBase* node);

// scene graph
void* bi_node_child_at(void* p,int index);
void* bi_node_add_node(void* parent,void* child);
void* bi_node_remove_node(void* parent,void* child);
void* bi_node_remove_from_parent(void* child);
int bi_node_get_z(void* n);
int bi_node_set_z(void* n, int z);

// Timer
BiTimer* bi_node_add_timer(void* node,BiTimer* timer);
BiTimer* bi_node_remove_timer(void* node,BiTimer* timer);

#endif // __BI_NODE_BASE_H__
