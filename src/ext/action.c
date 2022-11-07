#include <bi/ext/action.h>
#include <bi/context.h>
#include <stdlib.h>

typedef struct {
  BiNode *node;
  BiAction* action;
} ActionDoer;

BiAction* bi_action_init(BiAction *action)
{
  action->start = NULL;
  action->update = NULL;
  action->on_finish = NULL;
  action->state = BI_ACTION_STATE_READY;
  action->duration = 0;
  action->progress = 0.0;
  action->action_data = NULL;
  action->node = NULL;
  action->on_finish_callback_context = NULL;
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

  if(a->duration==0){
    a->progress = 1.0;
  }else{
    a->progress += (double)delta_time / a->duration;
  }
  if(a->progress<0.0){ a->progress = 0.0; }
  if(a->progress>1.0){ a->progress = 1.0; }

  a->update(a,a->progress,delta_time);

  if( a->progress >= 1.0 ) {
    if( a->state != BI_ACTION_STATE_FINISHED ) {
      a->state = BI_ACTION_STATE_FINISHED;
      if(a->on_finish) a->on_finish(a,a->on_finish_callback_context);
    }
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
