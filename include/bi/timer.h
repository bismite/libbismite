#ifndef __BI_CORE_TIMER_H__
#define __BI_CORE_TIMER_H__

#include <stdbool.h>
#include <stdint.h>

typedef struct _BiTimer BiTimer;
typedef struct _BiContext BiContext;

typedef bool (*timer_callback)(BiContext*,BiTimer*); // context,timer

struct _BiTimer {
    int repeat; // -1:infinity, 0:oneshot, 1:twice...
    int64_t interval; // msec
    int64_t will_fire_at; // msec
    int64_t last_fire_at; // msec
    timer_callback callback;
    void* userdata;
    int _state;
};

extern void bi_timer_init(BiTimer* timer, timer_callback callback, int64_t interval, int repeat, void* userdata);
extern void bi_finish_timer(BiTimer* timer);

// timers
typedef struct {
  int size;
  BiTimer **timers;
} BiTimers;

extern void bi_timers_init(BiTimers* timers);
extern void bi_run_timers(BiContext* context, BiTimers* timers);

extern void bi_add_timer(BiTimers* timers, BiTimer* timer);
extern BiTimer* bi_remove_timer(BiTimers* timers, BiTimer* timer);

#endif
