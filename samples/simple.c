#include <bi/bismite.h>

int main(int argc, char* argv[])
{
  BiContext* context = bi_init_context(malloc(sizeof(BiContext)), 480, 320, 0, true, __FILE__);
  // background color
  context->color = RGBA32(0x330000FF);
  // layer and root node
  BiLayer *layer = bi_layer_init(malloc(sizeof(BiLayer)));
  BiNode* root = &layer->root;
  bi_node_set_position(root,20,20);
  bi_node_set_size(root,200,200);
  root->color_tint = RGBA32(0x003300FF);
  bi_add_layer(context,layer);
  // start
  bi_start_run_loop(context);
  return 0;
}
