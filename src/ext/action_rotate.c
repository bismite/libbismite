#include <bi/ext/action.h>
#include <stdlib.h>

//
// Rotate By
//
static void bi_action_rotate_by_update(BiAction* action, double rate)
{
  BiActionRotate* rotate = (BiActionRotate*)action;
  double angle = rotate->from + rotate->angle * rate;
  bi_node_set_degree(action->node,angle);
}

static void bi_action_rotate_by_start(BiAction* action)
{
  BiActionRotate* rotate = (BiActionRotate*)action;
  rotate->from = bi_node_get_degree(action->node);
}

BiActionRotate* bi_action_rotate_by_init(BiActionRotate* rotate,int duration,double angle)
{
  bi_action_init(&rotate->action);
  rotate->action.update = bi_action_rotate_by_update;
  rotate->action.start = bi_action_rotate_by_start;
  rotate->action.duration = duration;
  rotate->angle = angle;
  return rotate;
}

//
// Rotate To
//
static void bi_action_rotate_to_update(BiAction* action, double rate)
{
  BiActionRotate* rotate = (BiActionRotate*)action;
  double angle = rotate->from + (rotate->angle - rotate->from) * rate;
  bi_node_set_degree(action->node,angle);
}

static void bi_action_rotate_to_start(BiAction* action)
{
  BiActionRotate* rotate = (BiActionRotate*)action;
  rotate->from = bi_node_get_degree(action->node);
}

BiActionRotate* bi_action_rotate_to_init(BiActionRotate* rotate,int duration,double angle)
{
  bi_action_init(&rotate->action);
  rotate->angle = angle;
  rotate->action.update = bi_action_rotate_to_update;
  rotate->action.start = bi_action_rotate_to_start;
  rotate->action.duration = duration;
  return rotate;
}
