#ifndef __BI_CORE_TIMER_H__
#define __BI_CORE_TIMER_H__

#include <stdbool.h>

struct _BiTimer;
typedef struct _BiTimer BiTimer;

typedef bool (*timer_callback)(double,BiTimer*); // now,timer

struct _BiTimer {
    int repeat; // -1:infinity, 0:oneshot, 1:twice...
    double interval;
    double will_fire_at;
    double last_fire_at;
    timer_callback callback;
    void* userdata;
    bool finished;
};

extern void bi_timer_init(BiTimer* timer, timer_callback callback, double interval, int repeat, void* userdata);
extern void bi_finish_timer(BiTimer* timer);

extern void bi_run_timers(int timers_size, BiTimer **timers, double now);

#endif
