#include <bi/ext/action.h>
#include <stdlib.h>

//
// Fade In
//
static void bi_action_fade_in_update(BiAction* action, double rate,int delta_time)
{
  BiActionFade* fade = action->action_data;
  action->node->color[3] = rate >= 1.0 ? 0xff : fade->from + (0xff - fade->from) * rate;
}

static void bi_action_fade_in_start(BiAction* action)
{
  BiActionFade* fade = action->action_data;
  fade->from = action->node->color[3];
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
  action->node->color[3] = rate >= 1.0 ? 0 : fade->from - fade->from * rate;
}

static void bi_action_fade_out_start(BiAction* action)
{
  BiActionFade* fade = action->action_data;
  fade->from = action->node->color[3];
}

void bi_action_fade_out_init(BiAction* action,int duration)
{
  action->update = bi_action_fade_out_update;
  action->start = bi_action_fade_out_start;
  action->duration = duration;
}