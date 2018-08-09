#ifndef __BI_CORE_PROFILE_H__
#define __BI_CORE_PROFILE_H__

#include <stdbool.h>

struct _BiProfile;
typedef struct _BiProfile BiProfile;

struct _BiProfile
{
    // FPS
    int target_fps;
    float current_fps;
    int frames;
    double frame_start_at;
    double last_frame_start_at;
    double delta;
    double delta_sum;
    double frame_spent;

    // statistics
    int matrix_updated;
    int total_matrix_updated;

    double stats_updated_at;

    struct {
      int actual_fps;
      float estimated_fps;
      float average_in_frame;
      float average_frame_to_frame;
      int total_matrix_updated;
    } stats;
};

extern void bi_profile_init(BiProfile* profile,int fps,double now);
extern void bi_frame_start(BiProfile* profile);
extern void bi_frame_end(BiProfile* profile);

#endif
