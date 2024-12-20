#include "common.h"

int main(int argc, char* argv[])
{
  BiContext* context = make_context(__FILE__);

  // Background
  BiShaderNode *shader_node = bi_shader_node_init(ALLOC(BiShaderNode));
  BiNode *root = bi_node_add_node(shader_node,make_bg("assets/check.png"));
  BiNode* face = make_sprite("assets/face01.png");
  bi_node_set_position(face,context->w/2,context->h/2);
  bi_node_set_scale(face,2.0,2.0);
  bi_node_add_node(root,face);
  shader_node->textures[0] = bi_node_get_texture(root);
  shader_node->textures[1] = bi_node_get_texture(face);
  shader_node->textures[2] = MAKE_TEXTURE("assets/noise.png");
  // Shader
  shader_node->shader = create_shader_with_default_vertex_shader("assets/shaders/dissolve.frag");
  bi_node_add_node(&context->default_framebuffer_node,shader_node);
  //
  bi_start_run_loop(context);
  return 0;
}
