#include <bi/ext/action.h>
#include <stdlib.h>

static void bi_action_repeat_start(BiNode* node, BiAction* action,double now)
{
  action->start_at = now;
  action->node = node;
  BiActionRepeat* rep = action->action_data;
  bi_action_start(node,rep->action,now);
}

static void bi_action_repeat_update(BiNode* node, BiAction* action, double rate)
{
  BiActionRepeat* rep = action->action_data;

  if(rate >= 1.0) {
    // finish
    bi_action_update( node, rep->action, 1.0 );
    // restart
    action->start_at += rep->action->duration;
    bi_action_start(node,rep->action,action->start_at);
    rate = 0;
  }

  bi_action_update( node, rep->action, rate );
}

void bi_action_repeat_init(BiAction* action,BiAction* target)
{
  BiActionRepeat* rep = action->action_data;
  rep->action = target;
  action->update = bi_action_repeat_update;
  action->start = bi_action_repeat_start;
  action->duration = target->duration;
  action->action_data = rep;
}
