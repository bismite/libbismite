#include "common.h"

int main(int argc, char* argv[])
{
  BiContext* context = bi_init_context(malloc(sizeof(BiContext)), 480, 320, 0, true, __FILE__);
  print_info(context);

  // Background
  BiLayer *bg = bi_layer_init(ALLOC(BiLayer));
  bg->root = make_sprite("assets/check.png");
  bi_node_set_position(bg->root,context->w/2,context->h/2);
  bg->textures[0] = bg->root->texture;
  bi_layer_group_add_layer(&context->layers,bg);

  // Foreground Layer Group
  BiLayer *fg = bi_layer_init(ALLOC(BiLayer));
  fg->root = make_sprite("assets/face01.png");
  bi_node_set_position(fg->root,context->w/2,context->h/2);
  bi_node_set_scale(fg->root,2.0,2.0);
  fg->textures[0] = fg->root->texture;
  // layer group
  BiLayerGroup* lg = bi_layer_group_init(malloc(sizeof(BiLayerGroup)));
  bi_layer_group_add_layer(lg,fg);
  bi_layer_group_add_layer_group(&context->layers,lg);

  // postprocess
  BiLayer *pp = bi_layer_init(ALLOC(BiLayer));
  pp->root = make_sprite("assets/noise.png");
  bi_node_set_position(pp->root,context->w/2, context->h/2);
  pp->textures[0] = pp->root->texture;
  pp->post_process.shader = create_shader_with_default_vertex_shader("assets/shaders/postprocess-dissolve.frag");
  pp->post_process.framebuffer_enabled = true;
  bi_set_blend_factor(&pp->post_process.blend_factor, GL_ONE,GL_ONE_MINUS_SRC_ALPHA,GL_ONE,GL_ONE_MINUS_SRC_ALPHA);

  bi_layer_group_add_layer(lg,pp);

  //
  bi_start_run_loop(context);
  return 0;
}
