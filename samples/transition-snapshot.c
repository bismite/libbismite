#include "common.h"
#include <bi/ext/transition.h>

#define TRANSITION_SPEED 0.002;
BiLayerGroup *group_a;
BiLayerGroup *group_b;

static void on_update_transition(BiTimer* t,double dt)
{
  BiLayer *transition_layer = t->userdata;
  transition_layer->shader_extra_data[0] += dt * TRANSITION_SPEED
  if( transition_layer->shader_extra_data[0] > 1.0 ) {
    transition_layer->shader_extra_data[0] = 1.0;
    bi_layer_remove_from_parent(transition_layer);
  }
}

static void on_update_rotate(BiTimer* t,double dt)
{
  BiNode* n = t->userdata;
  bi_node_set_angle(n,n->angle+dt*0.01);
}

static bool on_click(BiContext* context,BiNode* n, int x, int y, int button, bool pressed)
{
  if(pressed) return true;

  // remove current group
  bi_layer_group_remove_layer_group(&context->layers,group_a);
  // add next group
  bi_layer_group_add_layer_group(&context->layers,group_b);
  // make Transition
  BiShader* shader = create_shader_with_default_vertex_shader("assets/shaders/transition-stairs.frag");
  BiTransitionLayer *transition_layer = bi_transition_layer_init(ALLOC(BiTransitionLayer),
                                                                 context->w, context->h,
                                                                 shader,
                                                                 group_a, group_b,
                                                                 on_update_transition
                                                               );
  bi_add_layer(context, (BiLayer*)transition_layer);

  return true;
}

static BiLayerGroup* make_group_0(BiContext* context)
{
  BiLayer *layer = bi_layer_init(ALLOC(BiLayer));
  BiNode* bg = set_texture(&layer->root,"assets/check.png");
  BiNode* sprite = make_sprite("assets/face01.png");
  bi_node_set_position(sprite,context->w/2,context->h/2);
  bi_node_add_node(bg,sprite);
  bi_node_set_on_click(bg, on_click);
  onupdate(sprite,on_update_rotate);
  layer->textures[1] = bg->texture;
  layer->textures[0] = sprite->texture;
  BiLayerGroup* lg = bi_layer_group_init(ALLOC(BiLayerGroup));
  bi_layer_group_add_layer(lg,layer);
  return lg;
}

static BiLayerGroup* make_group_1(BiContext* context)
{
  BiLayer *layer = bi_layer_init(ALLOC(BiLayer));
  BiNode* bg = set_texture(&layer->root,"assets/map.png");
  BiNode* sprite = make_sprite("assets/mushroom.png");
  bi_node_set_position(sprite,context->w/2,context->h/2);
  bi_node_add_node(bg,sprite);
  onupdate(sprite,on_update_rotate);
  layer->textures[1] = bg->texture;
  layer->textures[0] = sprite->texture;
  BiLayerGroup* lg = bi_layer_group_init(ALLOC(BiLayerGroup));
  bi_layer_group_add_layer(lg,layer);
  return lg;
}

int main(int argc, char* argv[])
{
  BiContext* context = make_context(__FILE__);
  group_a = make_group_0(context);
  group_b = make_group_1(context);
  bi_layer_group_add_layer_group(&context->layers, group_a);
  bi_start_run_loop(context);
  return 0;
}
