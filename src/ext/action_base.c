#include <bi/ext/action.h>
#include <stdlib.h>

static void bi_action_base_update(BiNode* node, BiAction* action, double rate)
{
  // nop
}

static void bi_action_base_start(BiNode* node, BiAction* action,double now)
{
  action->start_at = now;
  action->node = node;
}

void bi_action_base_init(BiAction* action)
{
  action->update = bi_action_base_update;
  action->start = bi_action_base_start;
}
