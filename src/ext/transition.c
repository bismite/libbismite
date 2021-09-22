#include <bi/ext/transition.h>
#include <bi/context.h>
#include <bi/layer.h>

void bi_transition_init(BiTransition *transition,
                        BiLayerGroup *layer_group,
                        uint32_t duration,
                        bi_transition_callback callback,
                        BiShader* shader,
                        bool invert
                      )
{
  transition->_start_at = UINT64_MAX;
  transition->_done = false;
  transition->layer_group = layer_group;
  transition->duration = duration;
  transition->callback = callback;
  transition->invert = invert;
  transition->userdata = NULL;
  // shader
  transition->shader = shader;
  // layer
  bi_layer_init(&transition->layer);
  transition->layer.post_process.shader = transition->shader;
}

static bool transition_update(BiContext* context,BiTimer* timer)
{
  BiTransition *transition = timer->userdata;

  if(transition->_start_at == UINT64_MAX) {
    transition->_start_at = context->frame_start_at;
  }
  float progress = (context->frame_start_at - transition->_start_at) / (double)transition->duration;
  if(transition->_done){
    // finish
    bi_finish_timer(&transition->timer);
    bi_remove_timer(&context->timers,&transition->timer);
    bi_layer_group_remove_layer(transition->layer_group,&transition->layer);
    transition->layer_group->interaction_enabled = true;
    if(transition->callback){
      transition->callback(context,transition);
    }
  } else {
    // progress
    if( progress >= 1.0 ) {
      transition->_done = true;
    }
    transition->layer.post_process.shader_attributes[0] = transition->invert ? 1.0-progress : progress;
  }

  return true;
}

void bi_transition_start(BiContext* context, BiTransition* transition)
{
  transition->layer_group->interaction_enabled = false;
  bi_layer_group_add_layer(transition->layer_group,&transition->layer);
  transition->layer.post_process.shader_attributes[0] = transition->invert ? 1.0 : 0.0;
  transition->_start_at = UINT64_MAX;
  bi_timer_init(&transition->timer,transition_update,0,-1,transition);
  bi_add_timer(&context->timers,&transition->timer);
}