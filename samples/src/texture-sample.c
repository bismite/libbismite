#include "common.h"

int main(int argc, char* argv[])
{
  BiContext* context = bi_init_context(ALLOC(BiContext), 480, 320, 0, true, __FILE__);
  print_info(context);

  // Background
  BiNode* background = make_sprite_with_anchor("assets/check.png",0,0);

  // Face
  BiNode* face = make_sprite("assets/face01.png");
  bi_node_set_position(face,context->w/2,context->h/2);
  bi_node_set_size(face,64,64);
  bi_node_set_scale(face,2.0,2.0);
  bi_node_set_degree(face,45);
  bi_node_add_node(background,face);

  // layer
  BiLayer *layer = bi_layer_init(malloc(sizeof(BiLayer)));
  bi_add_layer(context,layer);
  layer->root = background;
  layer->textures[0] = background->_texture;
  layer->textures[1] = face->_texture;

  bi_start_run_loop(context);
  return 0;
}
