#ifndef __BI_NODE_BASE_H__
#define __BI_NODE_BASE_H__

#include <stdbool.h>
#include <bi/timer.h>
#include <bi/array.h>

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
  void* userdata;

struct _BiNodeBase{
  BI_NODE_HEADER
};

extern void bi_node_base_init(BiNodeBase* node,BiNodeClass class);
extern void bi_node_base_deinit(BiNodeBase* node);

static inline int bi_node_get_z(void* n) {
    return ((BiNodeBase*)n)->_z;
}
static inline void bi_node_set_z(void* n, int z)
{
  ((BiNodeBase*)n)->_z = z;
  if( ((BiNodeBase*)n)->parent ) {
    ((BiNodeBase*)n)->parent->children.order_cached = false;
  }
}

//
// scene graph
//
static inline BiNodeBase* bi_node_base_p(void* n){
  return n;
}
static inline void* bi_node_child_at(void* p,int index) {
  BiNodeBase* n = bi_node_base_p(p);
  return array_object_at(&n->children,index);
}
static inline void* bi_node_add_node(void* parent,void* child) {
  BiNodeBase* p = bi_node_base_p(parent);
  BiNodeBase* c = bi_node_base_p(child);
  array_add_object(&p->children,c);
  c->parent = p;
  if( p->_on_add_child ) { p->_on_add_child(p,c); }
  return child;
}
static inline void* bi_node_remove_at(void* parent,int index) {
  BiNodeBase* p = bi_node_base_p(parent);
  BiNodeBase *c = array_remove_object_at(&p->children,index);
  if(c==NULL) return NULL;
  c->parent = NULL;
  return c;
}
static inline void* bi_node_remove_node(void* parent,void* child) {
  BiNodeBase* p = bi_node_base_p(parent);
  BiNodeBase* c = bi_node_base_p(child);
  if( c == array_remove_object(&p->children,c) ){
    c->parent = NULL;
  }
  return c;
}
static inline void* bi_node_remove_from_parent(void* child) {
  BiNodeBase* c = bi_node_base_p(child);
  if(c && c->parent) {
    return bi_node_remove_node(c->parent,c);
  }
  return NULL;
}

#endif // __BI_NODE_BASE_H__
