#include <bi/ext/action.h>
#include <bi/context.h>
#include <stdlib.h>

BiAction* bi_action_init(BiAction *action)
{
  action->start = NULL;
  action->update = NULL;
  action->on_finish = NULL;
  action->state = BI_ACTION_STATE_READY;
  action->duration = 0;
  action->progress = 0.0;
  action->node = NULL;
  action->on_finish_callback_context = NULL;
  action->repeat = 1;
  action->repeat_count = 1;
  return action;
}

void bi_action_start(BiAction *action)
{
  action->state = BI_ACTION_STATE_RUNNING;
  action->start(action);
}

static void do_actions(BiTimer* timer,double delta_time)
{
  BiAction* a = timer->userdata;

  if(a->repeat_count==0) return;

  if(a->duration==0){
    a->progress = 1.0;
  }else{
    a->progress += delta_time / a->duration;
  }

  double remain = a->progress;
  bool dirty = false;
  bool done = false;
  while(remain>0){
    if(remain>=1.0){
      dirty = true;
      a->progress = 1.0;
      a->update(a,1.0);
      if(a->repeat_count>0) a->repeat_count -= 1;
      if(a->repeat_count==0) done = true;
      if(a->on_finish) a->on_finish(a,a->on_finish_callback_context);
      if(done) break;
    }else{
      dirty = false;
      a->progress = remain;
      a->update(a,remain);
    }
    if(dirty) a->start(a);
    remain -= 1.0;
  }
}

void bi_add_action(BiNode* node,BiAction* action)
{
  action->node = node;
  bi_timer_init(&action->timer, do_actions, 0, -1, action);
  bi_node_add_timer(node,&action->timer);
}

void bi_remove_action(BiNode* node, BiAction* action)
{
  bi_node_remove_timer(node,&action->timer);
}

void bi_action_set_repeat(BiAction *action,int repeat)
{
  action->repeat = repeat;
  action->repeat_count = repeat;
}
