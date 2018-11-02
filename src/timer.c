#include <bi/timer.h>
#include <bi/context.h>

void bi_timer_init(BiTimer* timer, timer_callback callback, double interval, int repeat, void* userdata)
{
    timer->repeat = repeat;
    timer->interval = interval;
    timer->will_fire_at = 0; // XXX: set at next frame start
    timer->last_fire_at = 0;
    timer->callback = callback;
    timer->userdata = userdata;
    timer->finished = false;
}

void bi_finish_timer(BiTimer* timer)
{
  timer->finished = true;
}


void bi_run_timers(int timers_size, BiTimer **timers, double now)
{
  int unfinished_timers = 0;
  for(int i=0;i<timers_size;i++){
    BiTimer* t = timers[i];

    // skip finished timer
    if(t->finished){
      continue;
    }

    // add in previous frame
    if(t->will_fire_at==0) {
      t->will_fire_at = now + t->interval;
    }

    if( now >= t->will_fire_at ) {
      t->callback(now,t);
      if(t->repeat == 0) {
        bi_finish_timer(t);
      }else{
        if(t->repeat > 0){
          t->repeat -= 1;
        }
        t->will_fire_at += t->interval;
      }
    }
    if(t->finished==false) {
      unfinished_timers++;
    }
  }
}