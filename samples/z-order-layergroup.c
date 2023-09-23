#include "common.h"

int main(int argc, char* argv[])
{
  BiContext* context = make_context(__FILE__);
  // foreground
  BiLayer *foreground_layer = bi_layer_init(ALLOC(BiLayer));
  BiNode* mushroom = bi_layer_add_node(foreground_layer,make_sprite("assets/mushroom.png"));
  bi_node_set_position(mushroom,W/2,H/2);
  foreground_layer->textures[0] = mushroom->texture;
  BiLayerGroup* foreground_layer_group = bi_layer_group_init(ALLOC(BiLayerGroup));
  bi_layer_group_add_layer(foreground_layer_group,foreground_layer);
  // background
  BiLayer* background_layer = bi_layer_init(ALLOC(BiLayer));
  BiNode* face = bi_layer_add_node(background_layer,make_sprite("assets/face01.png"));
  bi_node_set_position(face,W/2,H/2);
  background_layer->textures[0] = face->texture;
  BiLayerGroup* background_layer_group = bi_layer_group_init(ALLOC(BiLayerGroup));
  bi_layer_group_add_layer(background_layer_group,background_layer);
  // face on mushroom
  bi_layer_group_add_layer_group(&context->layers,foreground_layer_group);
  bi_layer_group_add_layer_group(&context->layers,background_layer_group);
  // mushroom on face
  background_layer_group->z = 0;
  foreground_layer_group->z = 1;
  // start
  bi_start_run_loop(context);
  return 0;
}
