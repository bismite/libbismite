#include <bi/ext/action.h>
#include <stdlib.h>

static void bi_action_rotate_by_update(BiNode* node, BiAction* action, double rate)
{
  BiActionRotateBy* rotate_by = action->action_data;
  double angle = rotate_by->from + rotate_by->by * rate;
  bi_node_set_degree(node,angle);
}

static void bi_action_rotate_by_start(BiNode* node, BiAction* action,double now)
{
  BiActionRotateBy* rotate_by = action->action_data;
  action->start_at = now;
  action->node = node;
  rotate_by->from = bi_node_get_degree(node);
}

void bi_action_rotate_by_init(BiAction* action,double duration,double angle)
{
  BiActionRotateBy* rotate_by = action->action_data;
  rotate_by->by = angle;
  action->update = bi_action_rotate_by_update;
  action->start = bi_action_rotate_by_start;
  action->duration = duration;
}
