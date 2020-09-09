#ifndef __BI_CORE_PROFILE_H__
#define __BI_CORE_PROFILE_H__

#include <stdbool.h>
#include <stdint.h>

struct _BiProfile;
typedef struct _BiProfile BiProfile;

typedef struct {
    int64_t start_at;
    int64_t end_at;
    int frames;
    float fps;
    int64_t time_spent_on_callback;
    int64_t time_spent_on_rendering;
} BiProfileStats;

struct _BiProfile
{
    int64_t profile_created_at;

    // FPS
    int target_fps;

    // current frame profile
    int64_t now;
    int matrix_updated;
    int rendering_nodes_queue_size;
    int on_update_callbacks_size;
    int callback_planned_nodes_size;
    int64_t time_spent_on_callback;
    int64_t time_spent_on_rendering;

    BiProfileStats _stats_tmp;
    BiProfileStats stats;
};

extern void bi_profile_init(BiProfile* profile,int fps,int64_t now);
extern void bi_profile_record(BiProfile* profile, int64_t now);

#endif
