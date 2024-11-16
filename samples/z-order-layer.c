#include "common.h"

int main(int argc, char* argv[])
{
  BiContext* context = make_context(__FILE__);
  context->color = RGBA(0x33,0,0x33,0xff);
  // background
  BiShaderNode *background_shader_node = bi_shader_node_init(ALLOC(BiShaderNode));
  BiNode* face = bi_shader_node_add_node(background_shader_node, make_sprite("assets/face01.png"));
  bi_node_set_position(face,W/2,H/2);
  background_shader_node->textures[0] = face->texture;
  // foreground
  BiShaderNode* foreground_shader_node = bi_shader_node_init(ALLOC(BiShaderNode));
  BiNode* mushroom = bi_shader_node_add_node(foreground_shader_node,make_sprite("assets/mushroom.png"));
  bi_node_set_position(mushroom,W/2,H/2);
  foreground_shader_node->textures[0] = mushroom->texture;
  // wrong order
  bi_add_shader_node(context,foreground_shader_node);
  bi_add_shader_node(context,background_shader_node);
  // fix by z order
  background_shader_node->z = 0;
  foreground_shader_node->z = 1;
  // start
  bi_start_run_loop(context);
  return 0;
}
