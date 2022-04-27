#include "common.h"

int main(int argc, char* argv[])
{
  BiContext* context = bi_init_context(ALLOC(BiContext), 480, 320, 0, true, __FILE__);
  print_info(context);

  // root node
  BiNode* root = bi_node_init(ALLOC(BiNode));

  // texture node 1
  BiNode* background = make_sprite("assets/check.png");
  bi_node_set_position(background,context->w/2,context->h/2);
  bi_node_add_node(root,background);

  // texture node 2
  BiTextureMapping* mapping = make_texture_mapping("assets/face01.png");
  bi_texture_mapping_set_bound(mapping,32,32,64,64);
  bi_texture_mapping_set_clip(mapping,32,32,128,128);

  BiNode* face =  bi_node_init(ALLOC(BiNode));
  bi_node_set_size(face, 128, 128);
  bi_node_set_anchor(face,0.5,0.5);
  face->texture_mapping = mapping;
  bi_node_set_matrix_include_anchor_translate(face,true);

  bi_node_set_position(face,context->w/2,context->h/2);
  bi_node_set_scale_x(face,2.0);
  bi_node_set_scale_y(face,2.0);
  bi_node_set_degree(face,45);
  bi_node_add_node(root,face);

  // texture node 3
  BiNode* tester = make_sprite("assets/tester.png");
  bi_node_set_position(tester,32,32);
  bi_node_set_scale_x(tester,0.25);
  bi_node_set_scale_y(tester,0.25);
  bi_node_add_node(face,tester);

  // layer
  BiLayer *layer = bi_layer_init(malloc(sizeof(BiLayer)));
  bi_add_layer(context,layer);
  layer->root = root;
  layer->textures[0] = background->texture_mapping->texture;
  layer->textures[1] = face->texture_mapping->texture;
  layer->textures[2] = tester->texture_mapping->texture;

  bi_start_run_loop(context);
  return 0;
}
