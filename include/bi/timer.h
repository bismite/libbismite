#ifndef __BI_CORE_TIMER_H__
#define __BI_CORE_TIMER_H__

#include <stdbool.h>

struct _BiContext;
typedef struct _BiContext BiContext;

struct _BiNode;
typedef struct _BiNode BiNode;

struct _BiTimer;
typedef struct _BiTimer BiTimer;

typedef bool (*timer_callback)(BiContext*,BiNode*,double,BiTimer*); // context,node,now,timer

struct _BiTimer {
    int repeat; // -1:infinity, 0:oneshot, 1:twice...
    double interval;
    double will_fire_at;
    double last_fire_at;
    BiNode* node;
    timer_callback callback;
    void* userdata;
    bool finished;
};

// timer
extern BiTimer* bi_timer_alloc(BiNode* node, timer_callback callback, double interval, int repeat, void* userdata);
extern void bi_add_timer(BiContext* context, BiTimer* timer);
extern void bi_finish_timer(BiContext* context, BiTimer* timer);
extern void bi_remove_timer(BiContext* context, BiTimer* timer);
extern void bi_free_timer(BiTimer* timer);
extern void bi_run_timers(BiContext *context, double now);

#endif
