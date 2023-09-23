#include "common.h"

#define NUM_OF_NODES 30000

static void rotate(BiTimer* t,double dt)
{
  BiNode *node = t->userdata;
  bi_node_set_angle(node, node->angle + 2.0*M_PI/180.0);
}

static BiNode* create_node(BiContext* c)
{
  BiNode* node = bi_node_init(malloc(sizeof(BiNode)));
  bi_node_set_anchor(node,0.5,0.5);
  bi_node_set_position(node, rand() % 320 - 320/2, rand() % 320 - 320/2 );
  bi_node_set_size(node, 2+rand()%20, 2+rand()%20 );
  bi_node_set_angle(node, (rand()%3600)/180.0 * M_PI );
  node->color_modulate = RGBA(rand()%0xff, rand()%0xff, rand()%0xff, 128);
  return node;
}

int main(int argc, char* argv[])
{
  BiContext* context = make_context(__FILE__);
  // layer
  BiLayer *layer = bi_layer_init(ALLOC(BiLayer));
  BiNode *root = bi_layer_add_node(layer,make_bg("assets/map.png"));
  layer->textures[0] = root->texture;
  bi_add_layer(context,layer);
  // nodes
  BiNode* center = bi_node_init(ALLOC(BiNode));
  bi_node_add_node(root, center);
  bi_node_set_position(center, context->w/2, context->h/2 );
  for(uint64_t i=0; i<NUM_OF_NODES ; i++){ bi_node_add_node(center, create_node(context)); }
  onupdate(center,rotate);
  // start
  bi_start_run_loop(context);
  return 0;
}
