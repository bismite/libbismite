#include <bi/ext/action.h>
#include <stdlib.h>

static void bi_action_rotate_to_update(BiNode* node, BiAction* action, double rate)
{
  BiActionRotateTo* rotate_to = action->action_data;
  double angle = rotate_to->from + (rotate_to->to - rotate_to->from) * rate;
  bi_node_set_degree(node,angle);
}

static void bi_action_rotate_to_start(BiNode* node, BiAction* action,double now)
{
  BiActionRotateTo* rotate_to = action->action_data;
  action->start_at = now;
  action->node = node;
  rotate_to->from = bi_node_get_degree(node);
}

void bi_action_rotate_to_init(BiAction* action,double duration,double angle)
{
  BiActionRotateTo* rotate_to = action->action_data;
  rotate_to->to = angle;
  action->update = bi_action_rotate_to_update;
  action->start = bi_action_rotate_to_start;
  action->duration = duration;
}
