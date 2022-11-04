#include <bi/node_base.h>
#include <stddef.h>
#include <stdlib.h>

void bi_raw_node_init(BiRawNode* node,BiNodeType type)
{
  node->type = type;
  node->z = 0;
  node->index = 0;
  node->interaction_enabled = true;
  // timers
  node->time_scale = 1.0;
  node->timers.size = 0;
  node->timers.timers = NULL;
  node->timers.scale = 1.0;
}

void bi_raw_node_add_timer(BiRawNode* node, BiTimer* timer)
{
  if(timer==NULL) return;
  if(timer->node) return;
  for(int i=0;i<node->timers.size;i++){
    if(node->timers.timers[i] == timer){
      return; // already
    }
  }
  timer->node = node;
  node->timers.size += 1;
  node->timers.timers = realloc(node->timers.timers, sizeof(BiTimer*)*node->timers.size);
  node->timers.timers[node->timers.size-1] = timer;
}

void bi_raw_node_remove_timer(BiRawNode* node, BiTimer* timer)
{
  if(timer==NULL) return;
  for(int i=0;i<node->timers.size;i++){
    if(node->timers.timers[i] == timer){
      node->timers.timers[i] = NULL;
      timer->node = NULL;
      return;
    }
  }
}

void bi_node_run_timers(BiContext* context,BiRawNode* node,double delta_time)
{
  delta_time *= node->timers.scale;

  for(int i=0;i<node->timers.size;i++){
    BiTimer* t = node->timers.timers[i];
    // skip
    if(t==NULL) continue;
    if(t->state == BI_TIMER_STATE_PAUSED) continue;
    if(t->count == 0) continue;
    // check schedule
    t->wait -= delta_time;
    if(t->wait < 0) {
      t->count -= 1;
      t->wait = t->interval;
      t->callback(context,t,delta_time);
    }
  }

  // remove and resize
  int new_size = 0;
  for(int i=0;i<node->timers.size;i++){
    BiTimer *t = node->timers.timers[i];
    if(t!=NULL && t->count != 0){
      node->timers.timers[new_size] = node->timers.timers[i];
      new_size++;
    }
  }
  if( node->timers.size != new_size ){
    node->timers.size = new_size;
    node->timers.timers = realloc( node->timers.timers, sizeof(BiTimer*) * node->timers.size );
  }
}
