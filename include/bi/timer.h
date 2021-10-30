#ifndef __BI_CORE_TIMER_H__
#define __BI_CORE_TIMER_H__

#include <stdint.h>

typedef struct _BiTimer BiTimer;
typedef struct _BiTimerManager BiTimerManager;
typedef struct _BiContext BiContext;

typedef void (*timer_callback)(BiContext*,BiTimer*,int);

typedef enum {
  BI_TIMER_STATE_PAUSED,
  BI_TIMER_STATE_RUNNING,
} BiTimerState;

struct _BiTimer {
  int count;
  int interval; // msec
  int wait; // msec
  timer_callback callback;
  void* userdata;
  BiTimerState state;
  BiTimerManager* manager;
};

extern void bi_timer_init(BiTimer* timer,
                          timer_callback callback,
                          int interval,
                          int count,
                          void* userdata);
extern void bi_timer_pause(BiTimer* timer);
extern void bi_timer_resume(BiTimer* timer);

struct _BiTimerManager {
  int size;
  BiTimer **timers;
  double scale;
};

extern void bi_timer_manager_init(BiTimerManager* timers);
extern void bi_timer_manager_run(BiContext* context, BiTimerManager* timers,int delta_time);
extern void bi_timer_manager_add_timer(BiTimerManager* timers, BiTimer* timer);
extern void bi_timer_manager_remove_timer(BiTimerManager* timers, BiTimer* timer);

#endif
