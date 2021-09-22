#include "common.h"

int main(int argc, char* argv[])
{
  BiContext* context = malloc(sizeof(BiContext));
  bi_init_context(context, 480, 320, 0, true, __FILE__);
  print_info(context);

  // Background
  BiLayer *bg = malloc(sizeof(BiLayer));
  bi_layer_init(bg);
  bg->root = make_sprite("assets/check.png");
  bi_node_set_position(bg->root,context->w/2,context->h/2);
  bg->textures[0] = bg->root->texture_mapping->texture;
  bi_layer_group_add_layer(&context->layers,bg);

  // Foreground Layer Group
  BiLayer *fg = malloc(sizeof(BiLayer));
  bi_layer_init(fg);
  fg->root = make_sprite("assets/face01.png");
  bi_node_set_position(fg->root,context->w/2,context->h/2);
  bi_node_set_scale(fg->root,2.0,2.0);
  fg->textures[0] = fg->root->texture_mapping->texture;
  // layer group
  BiLayerGroup* lg = malloc(sizeof(BiLayerGroup));
  bi_layer_group_init(lg);
  bi_layer_group_add_layer(lg,fg);
  bi_layer_group_add_layer_group(&context->layers,lg);

  // postprocess
  BiLayer *pp = malloc(sizeof(BiLayer));
  bi_layer_init(pp);
  pp->root = make_sprite("assets/noise.png");
  bi_node_set_position(pp->root,context->w/2, context->h/2);
  pp->textures[0] = pp->root->texture_mapping->texture;
  pp->post_process.shader = create_shader("assets/shaders/default.vert","assets/shaders/postprocess-dissolve.frag");
  pp->post_process.framebuffer_enabled = true;
  bi_set_blend_factor(&pp->post_process.blend_factor, GL_ONE,GL_ONE_MINUS_SRC_ALPHA,GL_ONE,GL_ONE_MINUS_SRC_ALPHA);

  bi_layer_group_add_layer(lg,pp);

  //
  bi_start_run_loop(context);
  return 0;
}
