#include <bi/timer.h>
#include <bi/context.h>

BiTimer* bi_timer_init(BiTimer* timer,
                       timer_callback callback,
                       int interval,
                       int count,
                       void* userdata)
{
  timer->count = count;
  timer->interval = interval;
  timer->wait = interval;
  timer->callback = callback;
  timer->userdata = userdata;
  timer->state = BI_TIMER_STATE_RUNNING;
  return timer;
}

void bi_timer_pause(BiTimer* timer)
{
  timer->state = BI_TIMER_STATE_PAUSED;
}

void bi_timer_resume(BiTimer* timer)
{
  timer->state = BI_TIMER_STATE_RUNNING;
}

BiTimers* bi_timers_init(BiTimers* timers)
{
  timers->size = 0;
  timers->timers = NULL;
  timers->scale = 1.0;
  return timers;
}

void bi_timers_add(BiTimers* timers, BiTimer* timer)
{
  if( !timers || !timer ) return;
  for(int i=0;i<timers->size;i++){
    if(timers->timers[i] == timer){
      return; // already
    }
  }
  timers->size += 1;
  timers->timers = realloc(timers->timers, sizeof(BiTimer*)*timers->size);
  timers->timers[timers->size-1] = timer;
}

void bi_timers_remove(BiTimers* timers, BiTimer* timer)
{
  if( !timers || !timer ) return;
  for(int i=0;i<timers->size;i++){
    if(timers->timers[i] == timer){
      timers->timers[i] = NULL;
      return;
    }
  }
}
