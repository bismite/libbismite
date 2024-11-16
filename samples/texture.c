#include "common.h"

int main(int argc, char* argv[])
{
  BiContext* context = make_context(__FILE__);

  // Background
  BiTexture* tex = MAKE_TEXTURE("assets/face01.png");
  BiNode* n = NULL;

  // shader_node
  BiShaderNode *shader_node = bi_shader_node_init(ALLOC(BiShaderNode));
  BiNode* background = make_bg("assets/check.png");
  bi_shader_node_add_node(shader_node,background);
  shader_node->textures[0] = background->texture;
  shader_node->textures[1] = tex;
  bi_add_shader_node(context,shader_node);

  // Angle
  n = make_sprite_from_texture(tex);
  bi_node_set_position(n,120,240);
  bi_node_set_degree(n,45);
  bi_shader_node_add_node(shader_node,n);

  // Flip Horizontal and Anchor
  n = make_sprite_from_texture(tex);
  bi_node_set_position(n,360,240);
  n->texture_flip_horizontal = true;
  bi_node_set_anchor(n,1,1);
  bi_shader_node_add_node(shader_node,n);

  // Flip Vertical and Resize
  n = make_sprite_from_texture(tex);
  bi_node_set_position(n,120,80);
  n->texture_flip_vertical = true;
  bi_node_set_size(n, 64,64);
  bi_shader_node_add_node(shader_node,n);

  // Scale
  n = make_sprite_from_texture(tex);
  bi_node_set_position(n,360,80);
  bi_node_set_scale(n, 0.8,1.2);
  bi_shader_node_add_node(shader_node,n);

  //
  bi_start_run_loop(context);
  return 0;
}
