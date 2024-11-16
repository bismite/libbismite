#include <bi/bismite.h>

int main(int argc, char* argv[])
{
  BiContext* context = bi_init_context(malloc(sizeof(BiContext)), 480, 320, 0, true, __FILE__);
  // shader_node and root node
  BiShaderNode *shader_node = bi_shader_node_init(malloc(sizeof(BiShaderNode)));
  BiNode* rect = bi_shader_node_add_node(shader_node,bi_node_init(malloc(sizeof(BiNode))));
  bi_node_set_position(rect,20,20);
  bi_node_set_size(rect,100,100);
  rect->color = RGBA(0xff,0xff,0,0xff);
  printf("%d,%d,%d,%d\n",rect->color.r, rect->color.g, rect->color.b, rect->color.a);
  bi_add_shader_node(context,shader_node);
  // start
  bi_start_run_loop(context);
  return 0;
}
