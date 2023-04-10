#include <bi/ext/action.h>
#include <stdlib.h>

//
// Move By
//
static void bi_action_move_by_update(BiAction* action, double rate)
{
  BiActionMove* move = (BiActionMove*)action;
  bi_node_set_position(action->node,
                       move->from_x + move->dx * rate,
                       move->from_y + move->dy * rate);
}

static void bi_action_move_by_start(BiAction* action)
{
  BiActionMove* move = (BiActionMove*)action;
  move->from_x = action->node->x;
  move->from_y = action->node->y;
}

BiActionMove* bi_action_move_by_init(BiActionMove* move,int duration,int dx,int dy)
{
  bi_action_init(&move->action);
  move->dx = dx;
  move->dy = dy;
  move->action.update = bi_action_move_by_update;
  move->action.start = bi_action_move_by_start;
  move->action.duration = duration;
  return move;
}

//
// Move To
//
static void bi_action_move_to_update(BiAction* action, double rate)
{
  BiActionMove* move = (BiActionMove*)action;
  int x = move->from_x + (move->dx - move->from_x) * rate;
  int y = move->from_y + (move->dy - move->from_y) * rate;
  bi_node_set_position(action->node,x,y);
}

static void bi_action_move_to_start(BiAction* action)
{
  BiActionMove* move = (BiActionMove*)action;
  move->from_x = action->node->x;
  move->from_y = action->node->y;
}

BiActionMove* bi_action_move_to_init(BiActionMove* move,int duration,int dx,int dy)
{
  bi_action_init(&move->action);
  move->dx = dx;
  move->dy = dy;
  move->action.update = bi_action_move_to_update;
  move->action.start = bi_action_move_to_start;
  move->action.duration = duration;
  return move;
}
