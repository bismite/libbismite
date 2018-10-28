#include <bi/timer.h>
#include <bi/context.h>

BiTimer* bi_timer_alloc(BiNode* node, timer_callback callback, double interval, int repeat, void* userdata)
{
    BiTimer *timer = malloc(sizeof(BiTimer));
    timer->repeat = repeat;
    timer->interval = interval;
    timer->will_fire_at = 0; // XXX: set at next frame start
    timer->last_fire_at = 0;
    timer->node = node;
    timer->callback = callback;
    timer->userdata = userdata;
    timer->finished = false;
    return timer;
}

void bi_add_timer(BiContext* context, BiTimer* timer)
{
    // TODO: duplicate check
    context->timers_size += 1;
    context->timers = realloc(context->timers, sizeof(BiTimer*)*context->timers_size);
    context->timers[context->timers_size-1] = timer;
}

void bi_remove_timer(BiContext* context, BiTimer* timer)
{

}

void bi_finish_timer(BiContext* context, BiTimer* timer)
{
  timer->finished = true;
}

void bi_free_timer(BiTimer* timer)
{
  free(timer);
}

void bi_run_timers(BiContext *context, double now)
{
  int unfinished_timers = 0;
  for(int i=0;i<context->timers_size;i++){
    BiTimer* t = context->timers[i];

    // skip finished timer
    if(t->finished){
      continue;
    }

    // add in previous frame
    if(t->will_fire_at==0) {
      t->will_fire_at = now + t->interval;
    }

    if( now >= t->will_fire_at ) {
      t->callback(context,t->node,now,t);
      if(t->repeat == 0) {
        bi_finish_timer(context,t);
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

  // actually removes finished timers
  if(context->timers_size != unfinished_timers){
    BiTimer **new_timers = NULL;
    BiTimer **p;
    p = new_timers = malloc(sizeof(BiTimer*)*unfinished_timers);
    for(int i=0; i<context->timers_size; i++){
      if( context->timers[i]->finished ) {
        free(context->timers[i]);
      } else {
        *p = context->timers[i];
        p++;
      }
    }
    free(context->timers);
    context->timers_size = unfinished_timers;
    context->timers = new_timers;
  }
}