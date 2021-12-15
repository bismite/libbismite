#ifndef __BI_CORE_TIMER_H__
#define __BI_CORE_TIMER_H__

#include <stdint.h>

typedef struct _BiTimer BiTimer;
typedef struct _BiContext BiContext;
typedef struct _BiRawNode BiRawNode;

typedef void (*timer_callback)(BiContext*,BiTimer*,double);

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
  BiRawNode *node;
};

extern BiTimer* bi_timer_init(BiTimer* timer,
                              timer_callback callback,
                              int interval,
                              int count,
                              void* userdata);
extern void bi_timer_pause(BiTimer* timer);
extern void bi_timer_resume(BiTimer* timer);

#endif
