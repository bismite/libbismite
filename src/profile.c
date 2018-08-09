#include <bi/profile.h>
#include <bi/util.h>
#include <bi/context.h>

void bi_profile_init(BiProfile* profile,int fps,double now)
{
  profile->target_fps = fps;
  profile->current_fps = 0;
  profile->frames = 0;
  profile->frame_start_at = now;
  profile->last_frame_start_at = now;
  profile->delta = 0;
  profile->delta_sum = 0;
  profile->frame_spent = 0;

  // statistics
  profile->stats_updated_at = now;
  profile->matrix_updated = 0;
  profile->stats.actual_fps = 0;
  profile->stats.estimated_fps = 0;
  profile->stats.average_in_frame = 0;
  profile->stats.average_frame_to_frame = 0;
  profile->stats.total_matrix_updated = 0;
}

static void record_stats(BiProfile* profile)
{
  profile->stats.actual_fps = profile->frames;
  profile->stats.estimated_fps = 1000 / (profile->frame_spent / profile->frames);
  profile->stats.average_in_frame = profile->frame_spent / profile->frames;
  profile->stats.average_frame_to_frame = profile->delta_sum / profile->frames;
  profile->stats.total_matrix_updated = profile->total_matrix_updated;
}

void bi_frame_start(BiProfile* profile)
{
    profile->last_frame_start_at = profile->frame_start_at;
    profile->frame_start_at = bi_get_now();
    profile->delta = profile->frame_start_at - profile->last_frame_start_at;
    profile->delta_sum += profile->delta;
    profile->frames++;

    if( (profile->frame_start_at - profile->stats_updated_at) > 1000 ) {

        record_stats(profile);

        profile->current_fps = profile->frames;
        profile->delta_sum = 0;
        profile->frames = 0;
        profile->frame_spent = 0;
        profile->total_matrix_updated = 0;
        profile->stats_updated_at = profile->frame_start_at;
    }
}

void bi_frame_end(BiProfile* profile)
{
  profile->frame_spent += bi_get_now() - profile->frame_start_at;
  profile->total_matrix_updated += profile->matrix_updated;
}
