#include "common.h"

int main(int argc, char* argv[])
{
  BiContext* context = bi_init_context(ALLOC(BiContext),480,320,0,false,__FILE__);
  print_info(context);

  // Background
  BiLayer *bg_layer = bi_layer_init(ALLOC(BiLayer));
  bi_add_layer(context,bg_layer);
  bg_layer->root = make_sprite_with_anchor("assets/check.png",0,0);
  bg_layer->textures[3] = bg_layer->root->_texture;

  // A
  BiNode* tester = make_sprite("assets/tester.png");
  BiNode* face = make_sprite("assets/face01.png");
  bi_node_set_position(tester,480/4,320/2);
  bi_node_add_node(bg_layer->root,tester);
  bi_node_add_node(tester,face);
  bg_layer->textures[0] = tester->_texture;
  bg_layer->textures[1] = face->_texture;

  // B
  BiLayerGroup* lg = bi_layer_group_init(ALLOC(BiLayerGroup));
  BiLayer* layer = bi_layer_init(ALLOC(BiLayer));
  layer->root = bi_node_init(ALLOC(BiNode));
  BiNode* tester2 = make_sprite("assets/tester.png");
  BiNode* face2 = make_sprite("assets/face01.png");
  layer->textures[0] = tester2->_texture;
  layer->textures[1] = face2->_texture;
  bi_node_set_position(tester2,480/4*3,320/2);
  bi_node_add_node(layer->root, tester2);
  bi_node_add_node(tester2,face2);
  bi_layer_group_add_layer(lg,layer);
  bi_layer_group_add_layer_group(&context->layers,lg);

  bi_start_run_loop(context);
  return 0;
}
