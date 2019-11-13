#include <bi/timer.h>
#include <stdlib.h>

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

//
// Timers
//

void bi_run_timers(BiTimers* timers, double now)
{
  int unfinished_timers = 0;
  for(int i=0;i<timers->size;i++){
    BiTimer* t = timers->timers[i];

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

void bi_add_timer(BiTimers* timers, BiTimer* timer)
{
    // TODO: duplicate check
    timers->size += 1;
    timers->timers = realloc(timers->timers, sizeof(BiTimer*)*timers->size);
    timers->timers[timers->size-1] = timer;
}

BiTimer* bi_remove_timer_index(BiTimers* timers, int index)
{
  if(index<0) return NULL;
  if(timers->size==0) return NULL;

  BiTimer* tmp = timers->timers[index];

  for(int i=index;i<timers->size-1;i++){
    timers->timers[i] = timers->timers[i+1];
  }

  timers->size -= 1;
  timers->timers = realloc( timers->timers, sizeof(BiTimer*) * timers->size );

  return tmp;
}

BiTimer* bi_remove_timer(BiTimers* timers, BiTimer* timer)
{
  int index = -1;
  for(int i=0;i<timers->size;i++){
    if(timers->timers[i] == timer){
      index = i;
      break;
    }
  }
  if(index<0){
    return NULL;
  }
  return bi_remove_timer_index(timers,index);
}
