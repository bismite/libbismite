#include "common.h"

int main(int argc, char* argv[])
{
  BiContext* context = bi_init_context(ALLOC(BiContext), 480, 320, 0, true, __FILE__);
  context->color = RGBA(0x33,0,0x33,0xff);
  print_info(context);

  // root node
  BiNode* root = bi_node_init(ALLOC(BiNode));

  // texture node
  BiNode* face_a = make_sprite("assets/face01.png");
  BiNode* face_b = make_sprite("assets/face01.png");
  face_a->color_tint = RGBA32(0x00FF00FF); // Green
  face_b->color_tint = RGBA32(0x0000FFFF); // Blue
  bi_node_set_position(face_a,context->w/2,context->h/2);
  bi_node_set_position(face_b,context->w/2 + 50,context->h/2 + 50);

  bi_node_add_node(root,face_a); // Green
  bi_node_add_node(root,face_b); // Blue

  bi_node_set_z(face_a,1); // Green move to top

  // layer
  BiLayer *layer = bi_layer_init(ALLOC(BiLayer));
  bi_add_layer(context,layer);
  layer->root = root;
  layer->textures[0] = face_a->texture;

  bi_start_run_loop(context);
  return 0;
}
