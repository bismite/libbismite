#include <bi/ext/action.h>
#include <bi/context.h>
#include <stdlib.h>

typedef struct {
  BiNode *node;
  BiAction* action;
} ActionDoer;

void bi_action_init(BiAction *action)
{
  action->start = NULL;
  action->update = NULL;
  action->on_finish = NULL;
  action->finit = false;
  action->started = false;
  action->finished = false;
  action->start_at = 0;
  action->duration = 0;
  action->action_data = NULL;
  action->timer = NULL;
  action->node = NULL;
  action->on_finish_callback_context = NULL;
}

void bi_action_start(BiNode *node, BiAction *action,double now)
{
  action->start(node,action,now);
}

void bi_action_update(BiNode *node, BiAction *action, double rate)
{
  action->update(node,action,rate);
}

static bool do_actions(BiContext* context,BiTimer* timer)
{
  BiAction* a = timer->userdata;
  BiNode *node = a->node;
  double rate = 0;

  if(a->duration==0){
    rate = 1.0;
  }else{
    rate = (context->frame_start_at - a->start_at) / a->duration;
  }
  if(rate<0.0){ rate = 0.0; }
  if(rate>=1.0){ rate = 1.0; }
  bi_action_update(node,a,rate);

  if( rate >= 1.0 ) {
    if( a->finished == false && a->on_finish ) {
      a->on_finish(a,a->on_finish_callback_context);
    }
    a->finished = true;
  }
  return true;
}

void bi_add_action(BiNode* node,BiAction* action)
{
  action->node = node;
  bi_timer_init(action->timer, do_actions, 0, -1, action);
  bi_add_timer(&node->timers,action->timer);
}

void bi_remove_action(BiNode* node, BiAction* action)
{
  BiTimer *timer = action->timer;
  bi_remove_timer(&node->timers,timer);
}
