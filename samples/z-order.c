#include "common.h"

int main(int argc, char* argv[])
{
  BiContext* context = make_context(__FILE__);
  context->color = RGBA(0x33,0,0x33,0xff);
  // sprites
  BiNode* face = make_sprite("assets/face01.png");
  BiNode* mushroom = make_sprite("assets/mushroom.png");
  // layer
  BiLayer *layer = bi_layer_init(ALLOC(BiLayer));
  bi_add_layer(context,layer);
  layer->textures[0] = face->texture;
  layer->textures[1] = mushroom->texture;
  // texture node
  bi_node_set_position(mushroom,W/2,H/2);
  bi_node_set_position(face,W/2,H/2);
  // face on mushroom
  bi_layer_add_node(layer,mushroom);
  bi_layer_add_node(layer,face);
  // mushroom on face
  bi_node_set_z(mushroom,1);
  bi_node_set_z(face,0);
  // start
  bi_start_run_loop(context);
  return 0;
}
