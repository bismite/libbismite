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
  timer->node = NULL;
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
