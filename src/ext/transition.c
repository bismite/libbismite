#include <bi/ext/transition.h>
#include <bi/context.h>
#include <bi/layer.h>

void bi_transition_init(BiTransition *transition,
                        BiLayerGroup *layer_group,
                        double duration,
                        bi_transition_callback callback,
                        BiShader* shader,
                        bool invert
                      )
{
  transition->start_at = UINT64_MAX;
  transition->done = false;
  transition->layer_group = layer_group;
  transition->duration = duration;
  transition->callback = callback;
  transition->invert = invert;
  transition->userdata = NULL;
  transition->delay_count = 0;
  // shader
  transition->shader = shader;
  // layer
  bi_layer_init(&transition->layer);
  transition->layer.post_process.shader = transition->shader;
}

static void transition_update(BiContext* context,BiTimer* timer,double delta_time)
{
  BiTransition *transition = timer->userdata;

  if(transition->delay_count > 0){
    transition->delay_count -= 1;
    return;
  }

  transition->progress += delta_time / transition->duration;

  if(transition->done){
    // finish
    bi_layer_group_remove_timer(transition->layer_group,&transition->timer);
    bi_layer_group_remove_layer(transition->layer_group,&transition->layer);
    transition->layer_group->interaction_enabled = true;
    if(transition->callback){
      transition->callback(context,transition);
    }
  } else {
    // progress
    if( transition->progress >= 1.0 ) {
      transition->done = true;
    }
    transition->layer.post_process.shader_attributes[0] =
      transition->invert ? 1.0-transition->progress : transition->progress;
  }
}

void bi_transition_start(BiContext* context, BiTransition* transition)
{
  transition->layer_group->interaction_enabled = false;
  bi_layer_group_add_layer(transition->layer_group,&transition->layer);
  transition->layer.post_process.shader_attributes[0] = transition->invert ? 1.0 : 0.0;
  transition->start_at = UINT64_MAX;
  transition->delay_count = 1;
  transition->progress = 0.0;
  bi_timer_init(&transition->timer,transition_update,0,-1,transition);
  bi_layer_group_add_timer(transition->layer_group,&transition->timer);
}
