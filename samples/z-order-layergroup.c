#include "common.h"

int main(int argc, char* argv[])
{
  BiContext* context = make_context(__FILE__);
  context->color = RGBA(0x33,0,0x33,0xff);

  // foreground
  BiLayer *foreground_layer = bi_layer_init(ALLOC(BiLayer));
  BiNode* mushroom = bi_layer_add_node(foreground_layer,make_sprite("assets/mushroom.png"));
  bi_node_set_position(mushroom,W/2,H/2);
  foreground_layer->textures[0] = mushroom->texture;
  BiFramebufferNode* foreground_framebuffer_node = bi_framebuffer_node_init(ALLOC(BiFramebufferNode));
  bi_framebuffer_node_add_layer(foreground_framebuffer_node,foreground_layer);

  // background
  BiLayer* background_layer = bi_layer_init(ALLOC(BiLayer));
  BiNode* face = bi_layer_add_node(background_layer,make_sprite("assets/face01.png"));
  bi_node_set_position(face,W/2,H/2);
  background_layer->textures[0] = face->texture;
  BiFramebufferNode* background_framebuffer_node = bi_framebuffer_node_init(ALLOC(BiFramebufferNode));
  bi_framebuffer_node_add_layer(background_framebuffer_node,background_layer);

  // face on mushroom
  bi_framebuffer_node_add_framebuffer_node(&context->layers,foreground_framebuffer_node);
  bi_framebuffer_node_add_framebuffer_node(&context->layers,background_framebuffer_node);
  // mushroom on face
  background_framebuffer_node->z = 0;
  foreground_framebuffer_node->z = 1;

  // start
  bi_start_run_loop(context);
  return 0;
}
