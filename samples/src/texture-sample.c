#include "common.h"

int main(int argc, char* argv[])
{
  BiContext* context = bi_init_context(malloc(sizeof(BiContext)), 480, 320, 0, true, __FILE__);
  print_info(context);

  // root node
  BiNode* root = bi_node_init(malloc(sizeof(BiNode)));

  // texture node 1
  BiNode* background = make_sprite("assets/check.png");
  bi_node_set_position(background,context->w/2,context->h/2);
  bi_node_add_node(root,background);

  // texture node 2
  BiNode* face = make_sprite("assets/face01.png");
  bi_node_set_position(face,context->w/2,context->h/2);
  bi_node_set_scale_x(face,2.0);
  bi_node_set_scale_y(face,2.0);
  bi_node_set_degree(face,45);
  bi_node_add_node(root,face);

  // layer
  BiLayer *layer = bi_layer_init(malloc(sizeof(BiLayer)));
  bi_add_layer(context,layer);
  layer->root = root;
  layer->textures[0] = background->texture_mapping->texture;
  layer->textures[1] = face->texture_mapping->texture;

  bi_start_run_loop(context);
  return 0;
}
