#include <bi/node_base.h>
#include <stddef.h>
#include <stdlib.h>

static int node_order_compare(const void *_a, const void *_b )
{
  const BiNodeBase *a = *(BiNodeBase**)_a;
  const BiNodeBase *b = *(BiNodeBase**)_b;
  return a->z == b->z ? a->index - b->index : a->z - b->z;
}

static void node_sort(Array* a)
{
  if( a->order_cached ) return;
  for( int i=0; i<a->size; i++ ){ ((BiNodeBase*)array_object_at(a,i))->index = i; }
  qsort( a->objects, a->size, sizeof(void*), node_order_compare);
  a->order_cached = true;
}

void bi_node_base_init(BiNodeBase* node,BiNodeClass class)
{
  node->class = class;
  node->parent = NULL;
  array_init(&node->children);
  node->children.sort = node_sort;
  node->z = 0;
  node->index = 0;
  node->interaction_enabled = true;
  node->time_scale = 1.0;
  bi_timers_init(&node->timers);
}

void bi_node_base_deinit(BiNodeBase* node)
{
  array_clear(&node->children);
  bi_timers_clear(&node->timers);
}
