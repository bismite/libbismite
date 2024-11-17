#include "common.h"

int main(int argc, char* argv[])
{
  BiShaderNode* snode = NULL;
  BiContext* context = make_context(__FILE__);

  // Sprites
  BiNode* mushroom = make_sprite("assets/mushroom.png");
  bi_node_set_position(mushroom,W/2,H/2);
  BiNode* face = make_sprite("assets/face01.png");
  bi_node_set_position(face,W/2,H/2);

  // Framebuffer Node
  BiNode* fbnode = bi_node_init(ALLOC(BiNode));
  fbnode->framebuffer = bi_framebuffer_init(ALLOC(BiFramebuffer),W,H);
  fbnode->texture_flip_vertical = true;
  BiTexture* fbtex = bi_texture_init_with_framebuffer(ALLOC(BiTexture),fbnode->framebuffer);
  bi_node_set_texture(fbnode,fbtex,0,0,fbtex->w,fbtex->h);
  bi_node_set_size(fbnode,W,H);
  // 1. Mushroom
  bi_node_add_node(fbnode,mushroom);
  // 2. Face (however, shader draws directly to the frame buffer)
  snode = bi_shader_node_init(ALLOC(BiShaderNode));
  snode->textures[0] = bi_node_get_texture(face);
  bi_node_add_node(snode,face);
  bi_node_add_node(fbnode,snode);

  // draw framebuffer to default framebuffer
  snode = bi_shader_node_init(ALLOC(BiShaderNode));
  bi_node_add_node(snode,fbnode);
  bi_node_add_node(&context->default_framebuffer_node,snode);
  snode->textures[0] = bi_node_get_texture(fbnode);
  snode->textures[1] = bi_node_get_texture(mushroom);

  // start
  bi_start_run_loop(context);
  return 0;
}
