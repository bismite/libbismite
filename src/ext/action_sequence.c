#include <bi/ext/action.h>
#include <stdlib.h>

static void run(BiAction *a,double rate)
{
  if(a->state == BI_ACTION_STATE_FINISHED) return;
  if(rate<0.0) rate = 0.0;
  if(rate>1.0) rate = 1.0;

  if(a->state == BI_ACTION_STATE_READY){
    bi_action_start(a);
    a->state = BI_ACTION_STATE_RUNNING;
  }

  a->update(a,rate);

  if(rate >= 1.0) {
    if( a->state != BI_ACTION_STATE_FINISHED ) {
      a->state = BI_ACTION_STATE_FINISHED;
      if(a->on_finish) a->on_finish(a,a->on_finish_callback_context);
    }
  }
}

static void bi_action_sequence_update(BiAction* action, double rate)
{
  BiActionSequence* seq = (BiActionSequence*)action;

  //    t1                      t2
  //    |----------delta--------|
  // x     yx         yx     yx   y
  // |--a--||----a----||--a--||-a-|
  int t1 = seq->progress;
  int t2 = rate*action->duration;
  int x = 0;
  int y = 0;
  BiAction *a = NULL;
  for(int i=0;i<seq->actions_size;i++) {
    a = seq->actions[i];
    y += a->duration;
    if( t1 <= y && x <= t2 ){
      double local_rate = a->duration==0 ? 1.0 : (double)(t2-x)/a->duration;
      run(seq->actions[i], local_rate);
    }
    x = y;
  }
  seq->progress = t2;
}

static void bi_action_sequence_start(BiAction* action)
{
  BiActionSequence* seq = (BiActionSequence*)action;
  for(int i=0;i<seq->actions_size;i++) {
    seq->actions[i]->state = BI_ACTION_STATE_READY;
    seq->actions[i]->node = action->node;
  }
  seq->progress = 0;
}

BiActionSequence* bi_action_sequence_init(BiActionSequence* seq,size_t num,BiAction** actions)
{
  bi_action_init(&seq->action);
  seq->actions_size = num;
  seq->action.duration = 0;
  for(int i=0;i<num;i++) {
    seq->actions[i] = actions[i];
    seq->action.duration += actions[i]->duration;
  }
  seq->action.update = bi_action_sequence_update;
  seq->action.start = bi_action_sequence_start;
  return seq;
}
