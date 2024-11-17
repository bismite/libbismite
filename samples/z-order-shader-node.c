#include "common.h"

int main(int argc, char* argv[])
{
  BiContext* context = make_context(__FILE__);

  // background
  BiShaderNode *background_shader_node = bi_shader_node_init(ALLOC(BiShaderNode));
  BiNode* face = bi_node_add_node(background_shader_node, make_sprite("assets/face01.png"));
  bi_node_set_position(face,W/2,H/2);
  background_shader_node->textures[0] = bi_node_get_texture(face);
  // foreground
  BiShaderNode* foreground_shader_node = bi_shader_node_init(ALLOC(BiShaderNode));
  BiNode* mushroom = bi_node_add_node(foreground_shader_node,make_sprite("assets/mushroom.png"));
  bi_node_set_position(mushroom,W/2,H/2);
  foreground_shader_node->textures[0] = bi_node_get_texture(mushroom);
  // wrong order
  bi_node_add_node(&context->default_framebuffer_node,foreground_shader_node);
  bi_node_add_node(&context->default_framebuffer_node,background_shader_node);
  // fix by z order
  bi_node_set_z(background_shader_node,0);
  bi_node_set_z(foreground_shader_node,1);
  // start
  bi_start_run_loop(context);
  return 0;
}
