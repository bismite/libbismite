#include <bi/ext/action.h>
#include <stdlib.h>

static void run(BiNode* node, BiAction *a,double now,double start_at)
{
  double rate = 0;
  if(a->finished) {
    return;
  }
  if(now<start_at) {
    return;
  }
  if(a->duration==0) {
    rate = 1.0;
  } else {
    rate = (now-start_at) / a->duration;
    if( 1.0 <= rate) rate = 1.0;
  }
  if( 0.0 <= rate ) {
    if(a->started==false){
      bi_action_start(node,a,start_at);
      a->started = true;
    }
    bi_action_update(node,a,rate);
    if(rate >= 1.0) {
      if( a->finished == false && a->on_finish ) {
        a->on_finish(a,a->on_finish_callback_context);
      }
      a->finished = true;
    }
  }
}

static void bi_action_sequence_update(BiNode* node, BiAction* action, double rate)
{
  double now = action->start_at + action->duration * rate;
  BiActionSequence* seq = action->action_data;

  if( now < action->start_at ) {
    return;
  }

  double start_at = action->start_at;
  for(int i=0;i<seq->actions_size;i++) {
    run(node,seq->actions[i],now,start_at);
    start_at += seq->actions[i]->duration;
  }
}

static void bi_action_sequence_start(BiNode* node, BiAction* action,double now)
{
  BiActionSequence* seq = action->action_data;
  action->start_at = now;
  action->node = node;

  double action_start_at = now;
  for(int i=0;i<seq->actions_size;i++) {
    seq->actions[i]->start_at = action_start_at;
    seq->actions[i]->started = false;
    seq->actions[i]->finished = false;
    seq->actions[i]->node = node;
    action_start_at += seq->actions[i]->duration;
  }
}

void bi_action_sequence_init(BiAction* action,size_t num,BiAction** actions)
{
  BiActionSequence* seq = action->action_data;
  seq->actions_size = num;
  for(int i=0;i<num;i++) {
    seq->actions[i] = actions[i];
    action->duration += actions[i]->duration;
  }
  action->update = bi_action_sequence_update;
  action->start = bi_action_sequence_start;
}
