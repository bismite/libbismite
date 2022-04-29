#include <bi/ext/action.h>
#include <stdlib.h>

//
// Scale By
//
static void bi_action_scale_by_update(BiAction* action, double rate,int delta_time)
{
  BiActionScale* scale = action->action_data;
  bi_node_set_scale(action->node,
                    scale->from_x + scale->dx * rate,
                    scale->from_y + scale->dy * rate);
}

static void bi_action_scale_by_start(BiAction* action)
{
  BiActionScale* scale = action->action_data;
  scale->from_x = action->node->_scale_x;
  scale->from_y = action->node->_scale_y;
}

void bi_action_scale_by_init(BiAction* action,int duration,double dx,double dy)
{
  BiActionScale* scale = action->action_data;
  scale->dx = dx;
  scale->dy = dy;
  action->update = bi_action_scale_by_update;
  action->start = bi_action_scale_by_start;
  action->duration = duration;
}

//
// Scale To
//
static void bi_action_scale_to_update(BiAction* action, double rate,int delta_time)
{
  BiActionScale* scale = action->action_data;
  bi_node_set_scale(action->node,
                    scale->from_x + (scale->dx - scale->from_x) * rate,
                    scale->from_x + (scale->dx - scale->from_x) * rate);
}

static void bi_action_scale_to_start(BiAction* action)
{
  BiActionScale* scale = action->action_data;
  scale->from_x = action->node->_scale_x;
  scale->from_y = action->node->_scale_y;
}

void bi_action_scale_to_init(BiAction* action,int duration,double dx,double dy)
{
  BiActionScale* scale = action->action_data;
  scale->dx = dx;
  scale->dy = dy;
  action->update = bi_action_scale_to_update;
  action->start = bi_action_scale_to_start;
  action->duration = duration;
}
