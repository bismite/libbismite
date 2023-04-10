#include <bi/ext/action.h>
#include <stdlib.h>

static void bi_action_base_update(BiAction* action, double rate)
{
  // nop
}

static void bi_action_base_start(BiAction* action)
{
  // nop
}

void bi_action_base_init(BiAction* action)
{
  bi_action_init(action);
  action->update = bi_action_base_update;
  action->start = bi_action_base_start;
}
