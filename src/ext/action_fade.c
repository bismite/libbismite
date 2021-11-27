#include <bi/ext/action.h>
#include <stdlib.h>

//
// Fade In
//
static void bi_action_fade_in_update(BiAction* action, double rate,int delta_time)
{
  BiActionFade* fade = action->action_data;
  action->node->opacity = rate >= 1.0 ? 1.0 : fade->from + (1.0 - fade->from) * rate;
}

static void bi_action_fade_in_start(BiAction* action)
{
  BiActionFade* fade = action->action_data;
  fade->from = action->node->opacity;
}

void bi_action_fade_in_init(BiAction* action,int duration)
{
  action->update = bi_action_fade_in_update;
  action->start = bi_action_fade_in_start;
  action->duration = duration;
}

//
// Fade Out
//
static void bi_action_fade_out_update(BiAction* action, double rate,int delta_time)
{
  BiActionFade* fade = action->action_data;
  action->node->opacity = rate >= 1.0 ? 0 : fade->from - fade->from * rate;
}

static void bi_action_fade_out_start(BiAction* action)
{
  BiActionFade* fade = action->action_data;
  fade->from = action->node->opacity;
}

void bi_action_fade_out_init(BiAction* action,int duration)
{
  action->update = bi_action_fade_out_update;
  action->start = bi_action_fade_out_start;
  action->duration = duration;
}
