#include "common.h"

int main(int argc, char* argv[])
{
  BiContext* context = make_context(__FILE__);

  // Main Framebuffer
  BiNode* main = bi_node_init(ALLOC(BiNode));
  bi_node_set_size(main,W,H);
  main->framebuffer = bi_framebuffer_init(ALLOC(BiFramebuffer),W,H);
  BiTexture* tex = bi_texture_init_with_framebuffer(ALLOC(BiTexture), main->framebuffer);
  bi_node_set_texture(main,tex,0,0,tex->w,tex->h);
  main->texture_flip_vertical = true;
  // Main Shader
  BiShaderNode *snode = bi_shader_node_init(ALLOC(BiShaderNode));
  BiNode *bg = bi_node_add_node(snode, make_bg("assets/check.png"));
  BiNode* face = bi_node_add_node(snode, make_sprite("assets/face01.png"));
  bi_node_set_position(face,W/2,H/2);
  snode->textures[0] = bi_node_get_texture(bg);
  snode->textures[1] = bi_node_get_texture(face);
  bi_node_add_node(main,snode);

  // Post Process
  BiShader *shader = create_shader_with_default_vertex_shader("assets/shaders/postprocess-distortion.frag");
  snode = bi_shader_node_init(ALLOC(BiShaderNode));
  snode->shader = shader;
  bi_node_add_node(snode,main);
  snode->textures[0] = bi_node_get_texture(main);
  bi_node_add_node(&context->default_framebuffer_node,snode);

  // start
  bi_start_run_loop(context);
  return 0;
}
