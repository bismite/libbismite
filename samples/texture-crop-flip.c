#include "common.h"

int main(int argc, char* argv[])
{
  BiContext* context = make_context(__FILE__);
  // shader_node
  BiTexture* tex = MAKE_TEXTURE("assets/face01.png");
  BiShaderNode *shader_node = bi_shader_node_init(ALLOC(BiShaderNode));
  bi_node_add_node(&context->default_framebuffer_node,shader_node);
  BiNode* background = bi_node_add_node(shader_node,make_bg("assets/check.png"));
  shader_node->textures[0] = bi_node_get_texture(background);
  shader_node->textures[1] = tex;
  // Nodes
  BiNode* n = NULL;
  // Normal
  n = make_sprite_from_texture(tex);
  bi_node_set_position(n,100,240);
  bi_node_add_node(background,n);
  // Crop
  n = make_sprite_from_texture(tex);
  bi_node_set_position(n,180,240);
  bi_node_set_cropped_texture(n,tex, 64,32,64,96, 64,32,128,128 );
  bi_node_add_node(background,n);
  // Flip Horizontal
  n = make_sprite_from_texture(tex);
  bi_node_set_position(n,320,240);
  n->texture_flip_horizontal = true;
  bi_node_add_node(background,n);
  // Flip Horizontal and Crop
  n = make_sprite_from_texture(tex);
  bi_node_set_position(n,460,240);
  bi_node_set_cropped_texture(n,tex, 64,32,64,96, 64,32,128,128 );
  n->texture_flip_horizontal = true;
  bi_node_add_node(background,n);
  // Flip Vertical
  n = make_sprite_from_texture(tex);
  bi_node_set_position(n,100,80);
  n->texture_flip_vertical = true;
  bi_node_add_node(background,n);
  // Flip Vertical and Crop
  n = make_sprite_from_texture(tex);
  bi_node_set_position(n,180,80);
  n->texture_flip_vertical = true;
  bi_node_set_cropped_texture(n,tex, 64,32,64,96, 64,32,128,128 );
  bi_node_add_node(background,n);
  // Flip Vertical and Horizontal
  n = make_sprite_from_texture(tex);
  bi_node_set_position(n,320,80);
  n->texture_flip_vertical = true;
  n->texture_flip_horizontal = true;
  bi_node_add_node(background,n);
  // Crop and Flip and Flip
  n = make_sprite_from_texture(tex);
  bi_node_set_position(n,460,80);
  n->texture_flip_vertical = true;
  n->texture_flip_horizontal = true;
  bi_node_set_cropped_texture(n,tex, 64,32,64,96, 64,32,128,128 );
  bi_node_add_node(background,n);
  // start
  bi_start_run_loop(context);
  return 0;
}
