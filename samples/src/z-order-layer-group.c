#include "common.h"

int main(int argc, char* argv[])
{
  BiContext* context = bi_init_context(malloc(sizeof(BiContext)),480,320,0,true,__FILE__);

  // layer group 1
  BiNode* face = make_sprite("assets/face01.png");
  bi_node_set_position(face,context->w/2,context->h/2);
  BiLayer *layer = bi_layer_init(malloc(sizeof(BiLayer)));
  layer->root = face;
  layer->textures[0] = face->texture_mapping->texture;
  BiLayerGroup* lg = bi_layer_group_init(malloc(sizeof(BiLayerGroup)));
  bi_layer_group_add_layer(lg,layer);

  // layer group 2
  BiNode* bg = make_sprite("assets/check.png");
  bi_node_set_position(bg,context->w/2,context->h/2);
  BiLayer* layer2 = bi_layer_init(malloc(sizeof(BiLayer)));
  layer2->root = bg;
  layer2->textures[0] = bg->texture_mapping->texture;
  BiLayerGroup* lg2 = bi_layer_group_init(malloc(sizeof(BiLayerGroup)));
  bi_layer_group_add_layer(lg2,layer2);

  //
  bi_layer_group_add_layer_group(&context->layers,lg);
  bi_layer_group_add_layer_group(&context->layers,lg2);

  // z order
  lg->z = 1;
  lg2->z = 0;

  bi_start_run_loop(context);
  return 0;
}
