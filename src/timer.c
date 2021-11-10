#include <bi/timer.h>
#include <bi/context.h>
#include <stdlib.h>

void bi_timer_init(BiTimer* timer,
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
  timer->manager = NULL;
}

void bi_timer_pause(BiTimer* timer)
{
  timer->state = BI_TIMER_STATE_PAUSED;
}
void bi_timer_resume(BiTimer* timer)
{
  timer->state = BI_TIMER_STATE_RUNNING;
}

//
// TimerManager
//

void bi_timer_manager_init(BiTimerManager* timer_manager)
{
  timer_manager->size = 0;
  timer_manager->timers = NULL;
  timer_manager->scale = 1.0;
}

void bi_timer_manager_run(BiContext* context, BiTimerManager* timer_manager,double delta_time)
{
  delta_time *= timer_manager->scale;

  for(int i=0;i<timer_manager->size;i++){
    BiTimer* t = timer_manager->timers[i];
    // skip
    if(t==NULL) continue;
    if(t->state == BI_TIMER_STATE_PAUSED) continue;
    if(t->count == 0) continue;
    // check schedule
    t->wait -= delta_time;
    if(t->wait < 0) {
      t->callback(context,t,delta_time);
      t->wait = t->interval;
      t->count -= 1;
    }
  }

  // remove and resize
  int new_size = 0;
  for(int i=0;i<timer_manager->size;i++){
    BiTimer *t = timer_manager->timers[i];
    if(t!=NULL && t->count != 0){
      timer_manager->timers[new_size] = timer_manager->timers[i];
      new_size++;
    }
  }
  if( timer_manager->size != new_size ){
    timer_manager->size = new_size;
    timer_manager->timers = realloc( timer_manager->timers, sizeof(BiTimer*) * timer_manager->size );
  }
}

void bi_timer_manager_add_timer(BiTimerManager* timer_manager, BiTimer* timer)
{
  if(timer==NULL) return;
  if(timer->manager) return;
  timer->manager = timer_manager;
  for(int i=0;i<timer_manager->size;i++){
    if(timer_manager->timers[i] == timer){
      return; // already
    }
  }
  timer_manager->size += 1;
  timer_manager->timers = realloc(timer_manager->timers, sizeof(BiTimer*)*timer_manager->size);
  timer_manager->timers[timer_manager->size-1] = timer;
}

void bi_timer_manager_remove_timer(BiTimerManager* timer_manager, BiTimer* timer)
{
  if(timer==NULL) return;
  timer->manager = NULL;
  for(int i=0;i<timer_manager->size;i++){
    if(timer_manager->timers[i] == timer){
      timer_manager->timers[i] = NULL;
      return;
    }
  }
}
