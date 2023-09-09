#include "common.h"

int main(int argc, char* argv[])
{
  BiContext* context = bi_init_context(ALLOC(BiContext), 480, 320, 0, true, __FILE__);
  print_info(context);

  // Background
  BiLayer *layer = bi_layer_init(ALLOC(BiLayer));
  layer->root = make_sprite("assets/check.png");
  bi_node_set_position(layer->root,context->w/2,context->h/2);

  BiNode* face = make_sprite("assets/face01.png");
  bi_node_set_scale(face,2.0,2.0);
  bi_node_add_node(layer->root,face);

  layer->textures[0] = layer->root->texture;
  layer->textures[1] = face->texture;
  layer->textures[2] = MAKE_TEXTURE("assets/noise.png");

  // Shader
  layer->shader = create_shader_with_default_vertex_shader("assets/shaders/dissolve.frag");

  bi_add_layer(context,layer);

  //
  bi_start_run_loop(context);
  return 0;
}
