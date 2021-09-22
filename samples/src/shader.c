#include "common.h"

int main(int argc, char* argv[])
{
  BiContext* context = malloc(sizeof(BiContext));
  bi_init_context(context, 480, 320, 0, true, __FILE__);
  print_info(context);

  // Shaders
  BiShader *bg_shader = create_shader("assets/shaders/default.vert","assets/shaders/shader-yellow.frag");
  BiShader *bg_pp_shader = create_shader("assets/shaders/default.vert","assets/shaders/postprocess-ball-distortion.frag");
  BiShader *fg_shader = create_shader("assets/shaders/default.vert","assets/shaders/shader-chop.frag");
  BiShader *fg_pp_shader = create_shader("assets/shaders/default.vert","assets/shaders/postprocess-ball-negative.frag");
  BiShader *final_pp_shader = create_shader("assets/shaders/default.vert","assets/shaders/postprocess-blur.frag");

  // Middle Layer Group
  BiLayerGroup* layer_group = malloc(sizeof(BiLayerGroup));
  bi_layer_group_init(layer_group);
  bi_layer_group_add_layer_group(&context->layers,layer_group);

  // Background layer
  BiLayer *bg_layer = malloc(sizeof(BiLayer));
  bi_layer_init(bg_layer);
  bg_layer->root = malloc(sizeof(BiNode));
  bi_node_init(bg_layer->root);
  BiNode* bg = make_sprite("assets/check.png");
  bi_node_set_position(bg,context->w/2,context->h/2);
  bi_node_add_node(bg_layer->root,bg);
  bg_layer->textures[0] = bg->texture_mapping->texture;
  bg_layer->shader = bg_shader;
  bi_layer_group_add_layer(layer_group,bg_layer);
  // post process
  bg_layer->post_process.shader = bg_pp_shader;

  // Foreground layer
  BiNode* root = make_sprite("assets/face01.png");
  bi_node_set_position(root,context->w/2,context->h/2);
  BiLayer *fg_layer = malloc(sizeof(BiLayer));
  bi_layer_init(fg_layer);
  fg_layer->root = root;
  bi_node_set_scale(root,2.0,2.0);
  fg_layer->textures[0] = root->texture_mapping->texture;
  fg_layer->shader = fg_shader;
  bi_layer_group_add_layer(layer_group,fg_layer);
  fg_layer->post_process.shader = fg_pp_shader;

  // fps layer
  BiFontAtlas *font = load_font();
  add_fps_layer(context,font);

  // PostProcess on Top LayerGroup
  BiLayer *final_pp_layer = malloc(sizeof(BiLayer));
  bi_layer_init(final_pp_layer);
  final_pp_layer->post_process.shader = final_pp_shader;
  bi_layer_group_add_layer(&context->layers,final_pp_layer);

  //
  bi_start_run_loop(context);
  return 0;
}
