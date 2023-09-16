#include <bi/ext/transition.h>
#include <bi/node.h>

BiTransitionLayer* bi_transition_layer_init(BiTransitionLayer *transition_layer,
                                            int w, int h,
                                            BiShader* postprocess_shader,
                                            BiLayerGroup* group0,
                                            BiLayerGroup* group1,
                                            timer_callback callback
                                          )
{
  BiLayer* layer = (BiLayer*)transition_layer;
  bi_layer_init_as_postprocess(layer);
  transition_layer->group0 = group0;
  transition_layer->group1 = group1;
  // snapshot source framebuffer
  BiCanvas* snapshot = &transition_layer->snapshot_canvas;
  bi_canvas_init_with_framebuffer(snapshot, &group0->framebuffer);
  bi_texture_init_with_framebuffer(&transition_layer->snapshot_texture,&snapshot->framebuffer);
  bi_texture_init_with_framebuffer(&transition_layer->group1_framebuffer_texture, &group1->framebuffer);
  // shader and texture
  layer->shader = postprocess_shader;
  layer->textures[0] = &transition_layer->snapshot_texture;
  layer->textures[1] = &transition_layer->group1_framebuffer_texture;
  // root
  bi_node_init( &transition_layer->root );
  bi_node_set_size( &transition_layer->root,w,h);
  layer->root = &transition_layer->root;
  // timer
  bi_timer_init(&transition_layer->timer,callback,0,-1,layer);
  bi_layer_add_timer(layer,&transition_layer->timer);
  //
  return transition_layer;
}
