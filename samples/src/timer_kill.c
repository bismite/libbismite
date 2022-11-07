#include "common.h"

static void rotate(BiTimer* timer,double dt)
{
  BiNode *node = timer->userdata;
  bi_node_set_angle(node, node->angle + dt*0.01);
}

static void kill_rotate(BiTimer* timer,double dt)
{
  BiTimer *timer_rotate = timer->userdata;
  BiNode *node = timer_rotate->userdata;
  bi_node_remove_timer(node,timer_rotate);
  free(timer_rotate);
  bi_node_remove_timer(node,timer);
  free(timer);
}

int main(int argc,char* argv[])
{
  BiContext* context = bi_init_context(ALLOC(BiContext),480,320,0,false,__FILE__);
  print_info(context);
  // layer
  BiLayer *layer = bi_layer_init(ALLOC(BiLayer));
  bi_add_layer(context,layer);
  layer->root = make_sprite_with_anchor("assets/check.png",0,0);;
  BiNode* face = make_sprite("assets/face01.png");
  bi_node_set_position(face,context->w/2,context->h/2);
  bi_node_add_node(layer->root,face);
  layer->textures[0] = layer->root->texture;
  layer->textures[1] = face->texture;
  // rotate and kill
  BiTimer *timer_rotate = bi_node_add_timer(face,bi_timer_init(ALLOC(BiTimer),rotate,10,-1,face));
  bi_node_add_timer(face,bi_timer_init(ALLOC(BiTimer),kill_rotate,1000,1,timer_rotate));
  // fps layer
  BiFontAtlas *font = load_font();
  add_fps_layer(context,font);
  // start
  bi_start_run_loop(context);
  return 0;
}
