#include "common.h"

int main(int argc, char* argv[])
{
  BiContext* context = bi_init_context(malloc(sizeof(BiContext)),480,320,0,true,__FILE__);

  // layer 1
  BiNode* face = make_sprite("assets/face01.png");
  bi_node_set_position(face,context->w/2,context->h/2);
  BiLayer *layer = bi_layer_init(malloc(sizeof(BiLayer)));
  layer->root = face;
  layer->textures[0] = face->_texture;

  // layer 2
  BiNode* bg = make_sprite("assets/check.png");
  bi_node_set_position(bg,context->w/2,context->h/2);
  BiLayer* layer2 = bi_layer_init(malloc(sizeof(BiLayer)));
  layer2->root = bg;
  layer2->textures[0] = bg->_texture;

  //
  bi_add_layer(context,layer);
  bi_add_layer(context,layer2);

  // z order
  layer->z = 1;
  layer2->z = 0;

  bi_start_run_loop(context);
  return 0;
}
