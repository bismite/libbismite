#include <bi/ext/action.h>
#include <stdlib.h>

static void bi_action_repeat_start(BiAction* action)
{
  BiActionRepeat* rep = action->action_data;
  rep->action->node = action->node;
  bi_action_start(rep->action);
}

static void bi_action_repeat_update(BiAction* action, double rate,int delta_time)
{
  BiActionRepeat* rep = action->action_data;
  BiAction *a = rep->action;

  while(delta_time > 0){
    int to_end = action->duration - rep->cursor;
    if( to_end > delta_time ) {
      rep->cursor = rep->cursor+delta_time;
      rate = (double)rep->cursor / action->duration;
      a->update(a,rate,delta_time);
      delta_time = 0;
    }else{
      a->update(a,1.0,to_end);
      if( a->state != BI_ACTION_STATE_FINISHED && a->on_finish ){
        a->state = BI_ACTION_STATE_FINISHED;
        a->on_finish(a,a->on_finish_callback_context);
      }
      rep->cursor = 0;
      bi_action_start(rep->action);
      delta_time -= to_end;
    }
  }
}

void bi_action_repeat_init(BiAction* action,BiAction* target)
{
  BiActionRepeat* rep = action->action_data;
  rep->action = target;
  rep->cursor = 0;
  action->update = bi_action_repeat_update;
  action->start = bi_action_repeat_start;
  action->duration = target->duration;
  action->action_data = rep;
}
