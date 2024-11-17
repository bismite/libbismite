#include "common.h"
#include <bi/ext/action.h>

int main(int argc, char* argv[])
{
  BiContext* context = make_context(__FILE__);

  // shader_node
  BiShaderNode *shader_node = bi_shader_node_init(ALLOC(BiShaderNode));
  BiNode* root = bi_node_add_node(shader_node, make_bg("assets/check.png") );
  bi_node_add_node(&context->default_framebuffer_node,shader_node);

  // sprite
  BiNode* face = make_sprite("assets/face01.png");
  bi_node_set_position(face,240,160);
  bi_node_add_node(root,face);
  shader_node->textures[0] = bi_node_get_texture(root);
  shader_node->textures[1] = bi_node_get_texture(face);

  // action
  BiActionRotate* rot = bi_action_rotate_by_init(ALLOC(BiActionRotate),1000,360);
  bi_action_set_repeat((BiAction*)rot, 3 );
  bi_add_action(face,(BiAction*)rot);
  bi_action_start((BiAction*)rot);

  bi_start_run_loop(context);
  return 0;
}
