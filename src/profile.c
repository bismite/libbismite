#include <bi/profile.h>
#include <bi/util.h>
#include <bi/context.h>

void bi_profile_init(BiProfile* profile,int fps,int64_t now)
{
  profile->target_fps = fps;
  profile->profile_created_at = now;

  profile->frame_start_at = now;
  profile->matrix_updated = 0;
  profile->rendering_nodes_queue_size = 0;
  profile->on_update_callbacks_size = 0;
  profile->callback_planned_nodes_size = 0;
  profile->time_spent_on_callback = 0;
  profile->time_spent_on_rendering = 0;


  // stats
  profile->stats.start_at = 0;
  profile->stats.fps = 0;
  profile->stats.time_spent_on_callback = 0;
  profile->stats.time_spent_on_rendering = 0;

  // tmp
  profile->stats_tmp.start_at = 0;
  profile->stats_tmp.frames = 0;
  profile->stats_tmp.time_spent_on_callback = 0;
  profile->stats_tmp.time_spent_on_rendering = 0;
}

void bi_profile_record(BiProfile* profile, int64_t now)
{
  profile->frame_start_at = now;
  if( profile->stats_tmp.start_at == 0 ) {
    profile->stats_tmp.start_at = now;
  }

  // increments
  profile->stats_tmp.frames++;
  profile->stats_tmp.time_spent_on_callback += profile->time_spent_on_callback;
  profile->stats_tmp.time_spent_on_rendering += profile->time_spent_on_rendering;
  profile->stats_tmp.matrix_updated += profile->matrix_updated;

  // finalize
  if( ( profile->frame_start_at - profile->stats_tmp.start_at) > 1000 ) {
    profile->stats_tmp.end_at = now;
    profile->stats_tmp.fps = 1000.0f * profile->stats_tmp.frames / (now-profile->stats_tmp.start_at);

    // average
    profile->stats_tmp.time_spent_on_callback /= profile->stats_tmp.frames;
    profile->stats_tmp.time_spent_on_rendering /= profile->stats_tmp.frames;
    profile->stats_tmp.matrix_updated /= profile->stats_tmp.frames;

    // update stats
    profile->stats = profile->stats_tmp;
    // clear
    profile->stats_tmp.start_at = 0;
    profile->stats_tmp.frames = 0;
  }
}
