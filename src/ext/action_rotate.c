#include <bi/ext/action.h>
#include <stdlib.h>

//
// Rotate By
//
static void bi_action_rotate_by_update(BiAction* action, double rate,int delta_time)
{
  BiActionRotate* rotate = action->action_data;
  double angle = rotate->from + rotate->angle * rate;
  bi_node_set_degree(action->node,angle);
}

static void bi_action_rotate_by_start(BiAction* action)
{
  BiActionRotate* rotate = action->action_data;
  rotate->from = bi_node_get_degree(action->node);
}

void bi_action_rotate_by_init(BiAction* action,int duration,double angle)
{
  BiActionRotate* rotate = action->action_data;
  rotate->angle = angle;
  action->update = bi_action_rotate_by_update;
  action->start = bi_action_rotate_by_start;
  action->duration = duration;
}

//
// Rotate To
//
static void bi_action_rotate_to_update(BiAction* action, double rate,int delta_time)
{
  BiActionRotate* rotate = action->action_data;
  double angle = rotate->from + (rotate->angle - rotate->from) * rate;
  bi_node_set_degree(action->node,angle);
}

static void bi_action_rotate_to_start(BiAction* action)
{
  BiActionRotate* rotate = action->action_data;
  rotate->from = bi_node_get_degree(action->node);
}

void bi_action_rotate_to_init(BiAction* action,int duration,double angle)
{
  BiActionRotate* rotate = action->action_data;
  rotate->angle = angle;
  action->update = bi_action_rotate_to_update;
  action->start = bi_action_rotate_to_start;
  action->duration = duration;
}
