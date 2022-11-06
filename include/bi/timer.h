#ifndef __BI_CORE_TIMER_H__
#define __BI_CORE_TIMER_H__

#include <stdint.h>
#include <stddef.h>

typedef struct _BiTimer BiTimer;

typedef void (*timer_callback)(BiTimer*,double);

typedef enum {
  BI_TIMER_STATE_PAUSED,
  BI_TIMER_STATE_RUNNING,
} BiTimerState;

struct _BiTimer {
  int count;
  int interval; // msec
  int wait; // msec
  timer_callback callback;
  BiTimerState state;
  void* userdata;
};

typedef struct _BiTimers{
  int size;
  BiTimer **timers;
  double scale;
} BiTimers;

extern BiTimer* bi_timer_init(BiTimer* timer,
                              timer_callback callback,
                              int interval,
                              int count,
                              void* userdata);
extern void bi_timer_pause(BiTimer* timer);
extern void bi_timer_resume(BiTimer* timer);

extern BiTimers* bi_timers_init(BiTimers* timers);
extern void bi_timers_add(BiTimers* timers,BiTimer* timer);
extern void bi_timers_remove(BiTimers* timers,BiTimer* timer);

#endif
