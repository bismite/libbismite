#include "common.h"

int main(int argc, char* argv[])
{
  BiContext* context = bi_init_context(ALLOC(BiContext), 480, 320, 0, true, __FILE__);
  print_info(context);
  // background color
  context->color = RGBA32(0x330000FF);
  // layer and root node
  BiLayer *layer = bi_layer_init(ALLOC(BiLayer));
  BiNode* root = &layer->root;
  bi_node_set_position(root,20,20);
  bi_node_set_size(root,200,200);
  root->color_tint = RGBA32(0xff0000FF);
  bi_add_layer(context,layer);
  // start
  bi_start_run_loop(context);
  return 0;
}
