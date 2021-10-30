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
  rep->cursor = rep->cursor+delta_time;
  if(rep->cursor >= action->duration) {
    rep->action->update(rep->action,1.0,delta_time);
    bi_action_start(rep->action);
    delta_time = rep->cursor % action->duration;
  }
  rep->cursor = rep->cursor % action->duration;
  rate = (double)rep->cursor / action->duration;
  rep->action->update(rep->action,rate,delta_time);
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
