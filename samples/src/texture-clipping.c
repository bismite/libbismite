#include "common.h"

int main(int argc, char* argv[])
{
  BiContext* context = bi_init_context(ALLOC(BiContext), 480, 320, 0, true, __FILE__);
  print_info(context);

  // root node
  BiNode* root = bi_node_init(ALLOC(BiNode));

  // Background
  BiNode* background = make_sprite("assets/check.png");
  bi_node_set_position(background,context->w/2,context->h/2);
  bi_node_add_node(root,background);

  // Face
  BiTexture* face_texture = MAKE_TEXTURE("assets/face01.png");
  BiNode* face = bi_node_init(ALLOC(BiNode));
  bi_node_set_cropped_texture(face,face_texture, 32,0,64,128, 32,0,128,128 );
  // bi_node_set_matrix_include_anchor_translate(face,true);
  bi_node_set_size(face, 64,64);
  bi_node_set_anchor(face,0.5,0.5);
  bi_node_set_position(face,context->w/2,context->h/2);
  bi_node_set_scale(face,2.0,2.0);
  bi_node_set_degree(face,45);
  bi_node_add_node(root,face);

  // layer
  BiLayer *layer = bi_layer_init(ALLOC(BiLayer));
  bi_add_layer(context,layer);
  layer->root = root;
  layer->textures[0] = background->_texture;
  layer->textures[1] = face_texture;

  bi_start_run_loop(context);
  return 0;
}
