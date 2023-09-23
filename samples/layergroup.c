#include "common.h"

// - Context LayerGroup
//   - Layer A
//     - Sprite
//   - LayerGroup
//     - Layer B
//       - Sprite
int main(int argc, char* argv[])
{
  BiContext* context = make_context(__FILE__);
  // Layer A
  BiLayer *layer_a = bi_layer_init(ALLOC(BiLayer));
  bi_add_layer(context,layer_a);
  BiNode *bg = bi_layer_add_node(layer_a,make_bg("assets/check.png"));
  layer_a->textures[0] = bg->texture;
  // Sprites for Layer A
  BiNode* tester = make_sprite("assets/tester.png");
  bi_node_set_position(tester,480/4,320/2);
  bi_node_add_node(bg,tester);
  layer_a->textures[1] = tester->texture;
  // Layer B
  BiLayer* layer_b = bi_layer_init(ALLOC(BiLayer));
  BiNode* tester2 = bi_layer_add_node(layer_b,make_sprite("assets/tester.png"));
  layer_b->textures[0] = tester2->texture;
  bi_node_set_position(tester2,480/4*3,320/2);
  // Layer Group
  BiLayerGroup* lg = bi_layer_group_init(ALLOC(BiLayerGroup));
  bi_layer_group_add_layer(lg,layer_b);
  bi_layer_group_add_layer_group(&context->layers,lg);
  //
  bi_start_run_loop(context);
  return 0;
}
