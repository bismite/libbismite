#include "common.h"

int main(int argc, char* argv[])
{
  BiContext* context = bi_init_context(ALLOC(BiContext), 480, 320, 0, true, __FILE__);
  print_info(context);

  // Background
  BiNode* background = make_sprite_with_anchor("assets/check.png",0,0);

  //
  BiTexture* tex = MAKE_TEXTURE("assets/face01.png");
  BiNode* n = NULL;

  // Normal
  n = make_sprite_from_texture(tex);
  bi_node_set_position(n,120,240);
  bi_set_color(n->color, 0,0,0,0);
  bi_set_color(n->color2, 0xff,0xff,0xff,0xff);
  bi_node_add_node(background,n);

  // Screen Red
  n = make_sprite_from_texture(tex);
  bi_node_set_position(n,360,240);
  bi_set_color(n->color, 0xff,0,0,0xff);
  bi_set_color(n->color2, 0xff,0xff,0xff,0xff);
  bi_node_add_node(background,n);

  // Multiply Green
  n = make_sprite_from_texture(tex);
  bi_node_set_position(n,120,80);
  bi_set_color(n->color, 0,0,0,0);
  bi_set_color(n->color2, 0,0xff,0,0xff);
  bi_node_add_node(background,n);

  // Multiply Blue & Screen White
  n = make_sprite_from_texture(tex);
  bi_node_set_position(n,360,80);
  bi_set_color(n->color, 0xff,0xff,0xff,0xff);
  bi_set_color(n->color2, 0,0,0xff,0xff);
  bi_node_add_node(background,n);

  // layer
  BiLayer *layer = bi_layer_init(malloc(sizeof(BiLayer)));
  bi_add_layer(context,layer);
  layer->root = background;
  layer->textures[0] = background->texture;
  layer->textures[1] = tex;

  bi_start_run_loop(context);
  return 0;
}
