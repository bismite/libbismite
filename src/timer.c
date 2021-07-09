#include <bi/timer.h>
#include <bi/context.h>
#include <stdlib.h>

void bi_timer_init(BiTimer* timer, timer_callback callback, int64_t interval, int repeat, void* userdata)
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

void bi_run_timers(BiContext* context, BiTimers* timers)
{
  for(int i=0;i<timers->size;i++){
    BiTimer* t = timers->timers[i];

    // skip if NULL
    if(t==NULL){
      continue;
    }

    // skip finished timer
    if(t->finished){
      continue;
    }

    // add in previous frame
    if(t->will_fire_at==0) {
      t->will_fire_at = context->frame_start_at + t->interval;
    }

    if( context->frame_start_at >= t->will_fire_at ) {
      t->callback(context,t);
      if(t->repeat == 0) {
        bi_finish_timer(t);
      }else{
        if(t->repeat > 0){
          t->repeat -= 1;
        }
        t->will_fire_at += t->interval;
      }
    }
  }
  // compaction
  int actual_size = 0;
  for(int i=0;i<timers->size;i++){
    if(timers->timers[i]!=NULL){
      timers->timers[actual_size] = timers->timers[i];
      actual_size++;
    }
  }
  if( timers->size != actual_size ){
    timers->size = actual_size;
    timers->timers = realloc( timers->timers, sizeof(BiTimer*) * timers->size );
  }
}

void bi_add_timer(BiTimers* timers, BiTimer* timer)
{
    // TODO: duplicate check
    timers->size += 1;
    timers->timers = realloc(timers->timers, sizeof(BiTimer*)*timers->size);
    timers->timers[timers->size-1] = timer;
}

BiTimer* bi_remove_timer(BiTimers* timers, BiTimer* timer)
{
  BiTimer* tmp = NULL;
  for(int i=0;i<timers->size;i++){
    if(timers->timers[i] == timer){
      tmp = timers->timers[i];
      timers->timers[i] = NULL;
      break;
    }
  }
  return tmp;
}
