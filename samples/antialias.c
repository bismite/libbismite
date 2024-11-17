#include "common.h"

int main(int argc, char* argv[])
{
  BiContext* context = make_context(__FILE__);

  // Layer
  BiShaderNode *shader_node = bi_shader_node_init(ALLOC(BiShaderNode));
  bi_node_add_node(&context->default_framebuffer_node,shader_node);
  BiNode* root = bi_node_add_node(shader_node,make_bg("assets/check.png"));

  // Texture
  BiTexture *texture = MAKE_TEXTURE("assets/tester.png");
  bi_texture_set_anti_alias(texture,true); // AA enabled

  // Sprite
  BiNode* sprite = bi_node_init(ALLOC(BiNode));
  bi_node_set_texture(sprite,texture,32,32,64,64);
  bi_node_set_size(sprite,64,64);
  bi_node_set_anchor(sprite,0.5,0.5);
  bi_node_set_position(sprite,context->w/2,context->h/2);
  bi_node_set_scale_x(sprite,4.0);
  bi_node_set_scale_y(sprite,4.0);

  // shader_node
  bi_node_add_node(root,sprite);
  shader_node->textures[0] = bi_node_get_texture(root);
  shader_node->textures[1] = texture;

  bi_start_run_loop(context);
  return 0;
}
