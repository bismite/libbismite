#include <bi/ext/action.h>
#include <stdlib.h>

static void run(BiAction *a,double rate,int delta_time)
{
  if(a->state == BI_ACTION_STATE_FINISHED) return;
  if(rate<0.0) rate = 0.0;
  if(rate>1.0) rate = 1.0;

  if(a->state == BI_ACTION_STATE_READY){
    bi_action_start(a);
    a->state = BI_ACTION_STATE_RUNNING;
  }

  a->update(a,rate,delta_time);

  if(rate >= 1.0) {
    if( a->state != BI_ACTION_STATE_FINISHED && a->on_finish ) {
      a->state = BI_ACTION_STATE_FINISHED;
      a->on_finish(a,a->on_finish_callback_context);
    }
  }
}

static void bi_action_sequence_update(BiAction* action, double rate,int delta_time)
{
  BiActionSequence* seq = action->action_data;
  //    t1                      t2
  //    |-----------------------|
  // x     yx         yx     yx   y
  // |--a--||----a----||--a--||-a-|
  int t1 = seq->progress;
  int t2 = seq->progress + delta_time;
  int x = 0;
  int y = 0;
  BiAction *a = NULL;
  for(int i=0;i<seq->actions_size;i++) {
    a = seq->actions[i];
    y += a->duration;
    if( t1 <= y && x <= t2 ){
      double local_rate = a->duration==0 ? 1.0 : (double)(t2-x)/a->duration;
      run(seq->actions[i], local_rate, delta_time);
    }
    x = y;
  }
  seq->progress = t2;
}

static void bi_action_sequence_start(BiAction* action)
{
  BiActionSequence* seq = action->action_data;
  for(int i=0;i<seq->actions_size;i++) {
    seq->actions[i]->state = BI_ACTION_STATE_READY;
    seq->actions[i]->node = action->node;
  }
  seq->progress = 0;
}

void bi_action_sequence_init(BiAction* action,size_t num,BiAction** actions)
{
  BiActionSequence* seq = action->action_data;
  seq->actions_size = num;
  action->duration = 0;
  for(int i=0;i<num;i++) {
    seq->actions[i] = actions[i];
    action->duration += actions[i]->duration;
  }
  action->update = bi_action_sequence_update;
  action->start = bi_action_sequence_start;
}
