#include "common.h"

int main(int argc, char* argv[])
{
  BiContext* context = make_context(__FILE__);

  // Background
  BiTexture* tex = MAKE_TEXTURE("assets/face01.png");
  BiNode* n = NULL;

  // layer
  BiLayer *layer = bi_layer_init(ALLOC(BiLayer));
  BiNode* background = set_texture(&layer->root,"assets/check.png");
  layer->textures[0] = background->texture;
  layer->textures[1] = tex;
  bi_add_layer(context,layer);

  // Angle
  n = make_sprite_from_texture(tex);
  bi_node_set_position(n,120,240);
  bi_node_set_degree(n,45);
  bi_node_add_node(background,n);

  // Flip Horizontal and Anchor
  n = make_sprite_from_texture(tex);
  bi_node_set_position(n,360,240);
  n->texture_flip_horizontal = true;
  bi_node_set_anchor(n,1,1);
  bi_node_add_node(background,n);

  // Flip Vertical and Resize
  n = make_sprite_from_texture(tex);
  bi_node_set_position(n,120,80);
  n->texture_flip_vertical = true;
  bi_node_set_size(n, 64,64);
  bi_node_add_node(background,n);

  // Scale
  n = make_sprite_from_texture(tex);
  bi_node_set_position(n,360,80);
  bi_node_set_scale(n, 0.8,1.2);
  bi_node_add_node(background,n);

  //
  bi_start_run_loop(context);
  return 0;
}
