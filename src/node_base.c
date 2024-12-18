#include <bi/node_base.h>
#include <bi/array.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
static inline BiNodeBase* bi_node_base_p(void* n){
  return n;
}

// remove event handler
static void gather_queued_children(BiNodeBase* n, Array* q) {
  for(int i=0;i<n->children.size;i++) {
    BiNodeBase* child = n->children.objects[i];
    if( child == NULL ) continue;
    if( child->_timer_queue != NULL || child->_interaction_queue != NULL ){
      array_add_object(q,child);
    }
    gather_queued_children(child,q);
  }
}
static inline void on_node_removed(BiNodeBase* n)
{
  Array q;
  array_init(&q);
  if( n->_timer_queue != NULL || n->_interaction_queue != NULL ){
    array_add_object(&q,n);
  }
  gather_queued_children(n,&q);
  for(int i=0;i<q.size;i++){
    BiNodeBase* target = q.objects[i];
    if(target->_timer_queue){
      int i = array_object_index(target->_timer_queue,target);
      if(i>=0) {
        target->_timer_queue->objects[i] = NULL;
        target->_timer_queue = NULL;
      }
    }
    if(target->_interaction_queue){
      int i = array_object_index(target->_interaction_queue,target);
      if(i>=0) {
        target->_interaction_queue->objects[i] = NULL;
        target->_interaction_queue = NULL;
      }
    }
  }
}

//
// Sort Function
//
static int node_order_compare(const void *_a, const void *_b )
{
  const BiNodeBase *a = *(BiNodeBase**)_a;
  const BiNodeBase *b = *(BiNodeBase**)_b;
  return a->_z == b->_z ? a->index - b->index : a->_z - b->_z;
}
static void node_sort(Array* a)
{
  if( a->order_cached ) return;
  for( int i=0; i<a->size; i++ ){ ((BiNodeBase*)array_object_at(a,i))->index = i; }
  qsort( a->objects, a->size, sizeof(void*), node_order_compare);
  a->order_cached = true;
}

//
// Init
//
void bi_node_base_init(BiNodeBase* node,BiNodeClass class)
{
  node->class = class;
  node->parent = NULL;
  array_init(&node->children);
  node->children.sort = node_sort;
  node->_z = 0;
  node->index = 0;
  node->interaction_enabled = true;
  node->time_scale = 1.0;
  node->_on_add_child = NULL;
  node->_timer_queue = NULL;
  node->_interaction_queue = NULL;
  bi_timers_init(&node->timers);
}

void bi_node_base_deinit(BiNodeBase* node)
{
  array_clear(&node->children);
  bi_timers_clear(&node->timers);
}

//
// scene graph
//
void* bi_node_child_at(void* p,int index) {
  BiNodeBase* n = bi_node_base_p(p);
  return array_object_at(&n->children,index);
}

void* bi_node_add_node(void* parent,void* child) {
  BiNodeBase* p = bi_node_base_p(parent);
  BiNodeBase* c = bi_node_base_p(child);
  array_add_object(&p->children,c);
  c->parent = p;
  if( p->_on_add_child ) { p->_on_add_child(p,c); }
  return child;
}

void* bi_node_remove_at(void* parent,int index) {
  BiNodeBase* p = bi_node_base_p(parent);
  BiNodeBase *c = array_remove_object_at(&p->children,index);
  if(c==NULL) return NULL;
  c->parent = NULL;
  on_node_removed(c);
  return c;
}

void* bi_node_remove_node(void* parent,void* child) {
  BiNodeBase* p = bi_node_base_p(parent);
  BiNodeBase* c = bi_node_base_p(child);
  if( c == array_remove_object(&p->children,c) ){
    c->parent = NULL;
    on_node_removed(c);
  }
  return c;
}

void* bi_node_remove_from_parent(void* child) {
  BiNodeBase* c = bi_node_base_p(child);
  if(c && c->parent) {
    return bi_node_remove_node(c->parent,c);
  }
  return NULL;
}

int bi_node_get_z(void* n) {
    return ((BiNodeBase*)n)->_z;
}

int bi_node_set_z(void* n, int z)
{
  ((BiNodeBase*)n)->_z = z;
  if( ((BiNodeBase*)n)->parent ) {
    ((BiNodeBase*)n)->parent->children.order_cached = false;
  }
  return z;
}

//
// Timer
//
BiTimer* bi_node_add_timer(void* node,BiTimer* timer){
  return bi_timers_add(&((BiNodeBase*)node)->timers,timer);
}

BiTimer* bi_node_remove_timer(void* node,BiTimer* timer){
  return bi_timers_remove(&((BiNodeBase*)node)->timers,timer);
}
