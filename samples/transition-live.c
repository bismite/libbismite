#include "common.h"

#define TRANSITION_SPEED 0.002;

static void on_update_transition(BiTimer* t,double dt)
{
  static double time = 0.0;
  time += dt * TRANSITION_SPEED;
  BiLayer *transition = t->userdata;
  transition->shader_extra_data[0] = (1.0+sin(time));
  if( transition->shader_extra_data[0] < 0.0 ) transition->shader_extra_data[0] = 0.0;
  if( transition->shader_extra_data[0] > 1.0 ) transition->shader_extra_data[0] = 1.0;
}

static void on_update_rotate(BiTimer* t,double dt)
{
  BiNode* n = t->userdata;
  bi_node_set_angle(n,n->angle+dt*0.01);
}

static BiLayerGroup* make_group_0(BiContext* context)
{
  BiNode* sprite = make_sprite("assets/face01.png");
  bi_node_set_position(sprite,context->w/2,context->h/2);
  BiLayer *layer = bi_layer_init(ALLOC(BiLayer));
  BiNode* bg = set_texture(&layer->root, "assets/check.png");
  bi_node_add_node(bg,sprite);
  onupdate(sprite,on_update_rotate);
  layer->textures[1] = bg->texture;
  layer->textures[0] = sprite->texture;
  BiLayerGroup* lg = bi_layer_group_init(ALLOC(BiLayerGroup));
  bi_layer_group_add_layer(lg,layer);
  return lg;
}

static BiLayerGroup* make_group_1(BiContext* context)
{
  BiNode* sprite = make_sprite("assets/mushroom.png");
  bi_node_set_position(sprite,context->w/2,context->h/2);
  BiLayer *layer = bi_layer_init(ALLOC(BiLayer));
  BiNode* bg = set_texture(&layer->root,"assets/map.png");
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
  //
  BiLayerGroup *group_0 = make_group_0(context);
  BiLayerGroup *group_1 = make_group_1(context);
  bi_layer_group_add_layer_group(&context->layers, group_0);
  bi_layer_group_add_layer_group(&context->layers, group_1);
  // Transition
  BiLayer *transition = bi_layer_init_as_postprocess(ALLOC(BiLayer));
  transition->shader = create_shader_with_default_vertex_shader("assets/shaders/transition-fade.frag");
  transition->textures[0] = bi_texture_init_with_framebuffer(ALLOC(BiTexture),&group_0->framebuffer);
  transition->textures[1] = bi_texture_init_with_framebuffer(ALLOC(BiTexture),&group_1->framebuffer);
  bi_node_set_size(&transition->root,context->w,context->h);
  bi_add_layer(context,transition);
  BiTimer *t = bi_timer_init(ALLOC(BiTimer),on_update_transition,0,-1,transition);
  bi_layer_add_timer(transition,t);
  //
  bi_start_run_loop(context);
  return 0;
}
