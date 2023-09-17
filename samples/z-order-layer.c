#include "common.h"

int main(int argc, char* argv[])
{
  BiContext* context = make_context(__FILE__);
  // background
  BiLayer *background_layer = bi_layer_init(ALLOC(BiLayer));
  BiNode* face = set_texture(&background_layer->root, "assets/face01.png");
  bi_node_set_position(face,W/2,H/2);
  background_layer->textures[0] = face->texture;
  // foreground
  BiLayer* foreground_layer = bi_layer_init(ALLOC(BiLayer));
  BiNode* mushroom = set_texture(&foreground_layer->root,"assets/mushroom.png");
  bi_node_set_position(mushroom,W/2,H/2);
  foreground_layer->textures[0] = mushroom->texture;
  // wrong order
  bi_add_layer(context,foreground_layer);
  bi_add_layer(context,background_layer);
  // fix by z order
  background_layer->root.z = 0;
  foreground_layer->root.z = 1;
  // start
  bi_start_run_loop(context);
  return 0;
}
