#include <bi/bismite.h>

int main(int argc, char* argv[])
{
  BiContext* context = bi_init_context(malloc(sizeof(BiContext)), 480, 320, 0, true, __FILE__);
  // background color
  context->color = RGBA32(0x330000FF);
  // layer and root node
  BiLayer *layer = bi_layer_init(malloc(sizeof(BiLayer)));
  BiNode* rect = bi_layer_add_node(layer,bi_node_init(malloc(sizeof(BiNode))));
  bi_node_set_position(rect,20,20);
  bi_node_set_size(rect,200,200);
  rect->color = RGBA(0xff,0xff,0,128);
  printf("%d,%d,%d,%d\n",rect->color.r, rect->color.g, rect->color.b, rect->color.a);
  bi_add_layer(context,layer);
  // start
  bi_start_run_loop(context);
  return 0;
}
