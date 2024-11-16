#include "common.h"

int main(int argc, char* argv[])
{
  BiContext* context = make_context(__FILE__);
  context->color = RGBA(0x33,0,0x33,0xff);
  // sprites
  BiNode* face = make_sprite("assets/face01.png");
  BiNode* mushroom = make_sprite("assets/mushroom.png");
  // shader_node
  BiShaderNode *shader_node = bi_shader_node_init(ALLOC(BiShaderNode));
  bi_add_shader_node(context,shader_node);
  shader_node->textures[0] = face->texture;
  shader_node->textures[1] = mushroom->texture;
  // texture node
  bi_node_set_position(mushroom,W/2,H/2);
  bi_node_set_position(face,W/2,H/2);
  // face on mushroom
  bi_shader_node_add_node(shader_node,mushroom);
  bi_shader_node_add_node(shader_node,face);
  // mushroom on face
  bi_node_set_z(mushroom,1);
  bi_node_set_z(face,0);
  // start
  bi_start_run_loop(context);
  return 0;
}
