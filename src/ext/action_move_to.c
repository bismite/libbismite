#include <bi/ext/action.h>
#include <stdlib.h>

static void bi_action_move_to_update(BiNode* node, BiAction* action, double rate)
{
  BiActionMoveTo* move_to = action->action_data;
  int x = move_to->from_x + (move_to->to_x - move_to->from_x) * rate;
  int y = move_to->from_y + (move_to->to_y - move_to->from_y) * rate;
  bi_node_set_position(node,x,y);
}

static void bi_action_move_to_start(BiNode* node, BiAction* action,double now)
{
  BiActionMoveTo* move_to = action->action_data;
  action->start_at = now;
  action->node = node;
  move_to->from_x = node->x;
  move_to->from_y = node->y;
}

void bi_action_move_to_init(BiAction* action,double duration,int x,int y)
{
  BiActionMoveTo* move_to = action->action_data;
  move_to->to_x = x;
  move_to->to_y = y;
  action->update = bi_action_move_to_update;
  action->start = bi_action_move_to_start;
  action->duration = duration;
}
