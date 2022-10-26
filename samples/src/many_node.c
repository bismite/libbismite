#include "common.h"

static void rotate(BiContext* c,BiTimer* t,double dt)
{
  BiNode *node = (BiNode*)t->node;
  bi_node_set_angle(node, node->angle + 2.0*M_PI/180.0);
}

static BiNode* create_node(BiContext* c)
{
  BiNode* node = bi_node_init(malloc(sizeof(BiNode)));
  bi_node_set_anchor(node,0.5,0.5);
  // position, scale, size
  bi_node_set_position(node, rand() % 320 - 320/2, rand() % 320 - 320/2 );
  bi_node_set_size(node, 2+rand()%20, 2+rand()%20 );
  bi_node_set_angle(node, (rand()%3600)/180.0 * M_PI );
  // color
  bi_set_color(node->color, rand()%0xff, rand()%0xff, rand()%0xff, 32);
  return node;
}

int main(int argc, char* argv[])
{
  srand( bi_get_now() );
  BiContext* context = malloc(sizeof(BiContext));
  bi_init_context(context, 480, 320, 0, true, __FILE__);
  print_info(context);

  // layer
  BiLayer *layer = bi_layer_init(malloc(sizeof(BiLayer)));
  layer->root = make_sprite_with_anchor("assets/map.png",0,0);;
  layer->textures[0] = layer->root->texture;
  bi_add_layer(context,layer);

  BiNode* center = bi_node_init(malloc(sizeof(BiNode)));
  bi_node_add_node(layer->root, center);
  bi_node_set_position(center, context->w/2, context->h/2 );
  for(uint64_t i=0; i<300000 ; i++){ bi_node_add_node(center, create_node(context)); }
  onupdate(center,rotate);

  //
  // fps layer
  //
  BiFontAtlas *font = load_font();
  add_fps_layer(context,font);

  bi_start_run_loop(context);
  return 0;
}
