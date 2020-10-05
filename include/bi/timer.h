#ifndef __BI_CORE_TIMER_H__
#define __BI_CORE_TIMER_H__

#include <stdbool.h>
#include <stdint.h>

struct _BiTimer;
typedef struct _BiTimer BiTimer;

typedef bool (*timer_callback)(int64_t,BiTimer*); // now,timer

struct _BiTimer {
    int repeat; // -1:infinity, 0:oneshot, 1:twice...
    int64_t interval; // msec
    int64_t will_fire_at; // msec
    int64_t last_fire_at; // msec
    timer_callback callback;
    void* userdata;
    bool finished;
};

extern void bi_timer_init(BiTimer* timer, timer_callback callback, int64_t interval, int repeat, void* userdata);
extern void bi_finish_timer(BiTimer* timer);

// timers
typedef struct {
  int size;
  BiTimer **timers;
} BiTimers;

extern void bi_run_timers(BiTimers* timers, int64_t now);

extern void bi_add_timer(BiTimers* timers, BiTimer* timer);
extern BiTimer* bi_remove_timer(BiTimers* timers, BiTimer* timer);

#endif
