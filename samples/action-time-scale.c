#include "common.h"
#include <bi/ext/action.h>

void rotate(BiNode* n) {
  BiAction* rot = (BiAction*)bi_action_rotate_by_init(ALLOC(BiActionRotate),0,2);
  bi_action_set_repeat(rot,-1);
  bi_add_action(n,rot);
  bi_action_start(rot);
}

int main(int argc, char* argv[])
{
  BiContext* context = make_context(__FILE__);
  // shader_node
  BiTexture *tex = MAKE_TEXTURE("assets/face01.png");
  BiShaderNode *shader_node = bi_shader_node_init(ALLOC(BiShaderNode));
  bi_node_add_node(&context->default_framebuffer_node,shader_node);
  BiNode* bg = bi_node_add_node(shader_node,make_bg("assets/check.png"));
  shader_node->textures[0] = bi_node_get_texture(bg);
  shader_node->textures[1] = tex;
  // Blue
  BiNode* blue = make_sprite_from_texture(tex);
  blue->tint = RGBA(0,0,0xFF,0xFF);
  bi_node_set_position(blue,100,100);
  rotate(blue);
  bi_node_add_node(bg,blue);
  blue->time_scale = 0.0; // STOP
  // Red
  BiNode* red = make_sprite_from_texture(tex);
  red->tint = RGBA(0xFF,0,0,0xFF);
  bi_node_set_position(red,300,100);
  rotate(red);
  bi_node_add_node(bg,red);

  // start
  bi_start_run_loop(context);
  return 0;
}
