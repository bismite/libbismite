#include <bi/ext/action.h>
#include <stdlib.h>

//
// Scale By
//
static void bi_action_scale_by_update(BiAction* action, double rate)
{
  BiActionScale* scale = (BiActionScale*)action;
  bi_node_set_scale(action->node,
                    scale->from_x + (scale->from_x*scale->dx-scale->from_x) * rate,
                    scale->from_y + (scale->from_y*scale->dy-scale->from_y) * rate );
}

static void bi_action_scale_by_start(BiAction* action)
{
  BiActionScale* scale = (BiActionScale*)action;
  scale->from_x = action->node->scale_x;
  scale->from_y = action->node->scale_y;
}

BiActionScale* bi_action_scale_by_init(BiActionScale* scale,int duration,double dx,double dy)
{
  bi_action_init(&scale->action);
  scale->dx = dx;
  scale->dy = dy;
  scale->action.update = bi_action_scale_by_update;
  scale->action.start = bi_action_scale_by_start;
  scale->action.duration = duration;
  return scale;
}

//
// Scale To
//
static void bi_action_scale_to_update(BiAction* action, double rate)
{
  BiActionScale* scale = (BiActionScale*)action;
  bi_node_set_scale(action->node,
                    scale->from_x + (scale->dx-scale->from_x) * rate,
                    scale->from_x + (scale->dx-scale->from_x) * rate);
}

static void bi_action_scale_to_start(BiAction* action)
{
  BiActionScale* scale = (BiActionScale*)action;
  scale->from_x = action->node->scale_x;
  scale->from_y = action->node->scale_y;
}

BiActionScale* bi_action_scale_to_init(BiActionScale* scale,int duration,double dx,double dy)
{
  bi_action_init(&scale->action);
  scale->dx = dx;
  scale->dy = dy;
  scale->action.update = bi_action_scale_to_update;
  scale->action.start = bi_action_scale_to_start;
  scale->action.duration = duration;
  return scale;
}
