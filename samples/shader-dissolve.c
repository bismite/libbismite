#include "common.h"

int main(int argc, char* argv[])
{
  BiContext* context = bi_init_context(ALLOC(BiContext), 480, 320, 0, true, __FILE__);
  print_info(context);
  // Background
  BiLayer *layer = bi_layer_init(ALLOC(BiLayer));
  BiNode *root = bi_layer_add_node(layer,make_bg("assets/check.png"));
  BiNode* face = make_sprite("assets/face01.png");
  bi_node_set_position(face,context->w/2,context->h/2);
  bi_node_set_scale(face,2.0,2.0);
  bi_node_add_node(root,face);
  layer->textures[0] = root->texture;
  layer->textures[1] = face->texture;
  layer->textures[2] = MAKE_TEXTURE("assets/noise.png");
  // Shader
  layer->shader = create_shader_with_default_vertex_shader("assets/shaders/dissolve.frag");
  bi_add_layer(context,layer);
  //
  bi_start_run_loop(context);
  return 0;
}
