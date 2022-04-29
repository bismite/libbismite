#include <bi/ext/action.h>
#include <stdlib.h>

//
// Move By
//
static void bi_action_move_by_update(BiAction* action, double rate,int delta_time)
{
  BiActionMove* move = action->action_data;
  bi_node_set_position(action->node,
                       move->from_x + move->dx * rate,
                       move->from_y + move->dy * rate);
}

static void bi_action_move_by_start(BiAction* action)
{
  BiActionMove* move = action->action_data;
  move->from_x = action->node->_x;
  move->from_y = action->node->_y;
}

void bi_action_move_by_init(BiAction* action,int duration,int dx,int dy)
{
  BiActionMove* move = action->action_data;
  move->dx = dx;
  move->dy = dy;
  action->update = bi_action_move_by_update;
  action->start = bi_action_move_by_start;
  action->duration = duration;
}

//
// Move To
//
static void bi_action_move_to_update(BiAction* action, double rate,int delta_time)
{
  BiActionMove* move = action->action_data;
  int x = move->from_x + (move->dx - move->from_x) * rate;
  int y = move->from_y + (move->dy - move->from_y) * rate;
  bi_node_set_position(action->node,x,y);
}

static void bi_action_move_to_start(BiAction* action)
{
  BiActionMove* move = action->action_data;
  move->from_x = action->node->_x;
  move->from_y = action->node->_y;
}

void bi_action_move_to_init(BiAction* action,int duration,int dx,int dy)
{
  BiActionMove* move = action->action_data;
  move->dx = dx;
  move->dy = dy;
  action->update = bi_action_move_to_update;
  action->start = bi_action_move_to_start;
  action->duration = duration;
}
