#include <bi/ext/action.h>
#include <stdlib.h>

//
// Fade In
//
static void bi_action_fade_in_update(BiAction* action, double rate)
{
  BiActionFade* fade = (BiActionFade*)action;
  bi_node_set_opacity( action->node, rate >= 1.0 ? 0xff : fade->from + (0xff - fade->from) * rate );
}

static void bi_action_fade_in_start(BiAction* action)
{
  BiActionFade* fade = (BiActionFade*)action;
  fade->from = bi_node_get_opacity(action->node);
}

BiActionFade* bi_action_fade_in_init(BiActionFade* fade,int duration)
{
  bi_action_init(&fade->action);
  fade->action.update = bi_action_fade_in_update;
  fade->action.start = bi_action_fade_in_start;
  fade->action.duration = duration;
  return fade;
}

//
// Fade Out
//
static void bi_action_fade_out_update(BiAction* action, double rate)
{
  BiActionFade* fade = (BiActionFade*)action;
  bi_node_set_opacity( action->node, rate >= 1.0 ? 0 : fade->from - fade->from * rate );
}

static void bi_action_fade_out_start(BiAction* action)
{
  BiActionFade* fade = (BiActionFade*)action;
  fade->from = bi_node_get_opacity(action->node);
}

BiActionFade* bi_action_fade_out_init(BiActionFade* fade,int duration)
{
  bi_action_init(&fade->action);
  fade->action.update = bi_action_fade_out_update;
  fade->action.start = bi_action_fade_out_start;
  fade->action.duration = duration;
  return fade;
}
