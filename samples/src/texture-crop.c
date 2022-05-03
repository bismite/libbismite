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
  bi_node_add_node(background,n);
  // Crop
  n = make_sprite_from_texture(tex);
  bi_node_set_position(n,120,240);
  bi_node_set_cropped_texture(n,tex, 64,32,64,96, 64,32,128,128 );
  bi_node_add_node(background,n);

  // Flip Horizontal (Crop)
  n = make_sprite_from_texture(tex);
  bi_node_set_position(n,360,240);
  bi_node_set_cropped_texture(n,tex, 64,32,64,96, 64,32,128,128 );
  n->_texture_flip_horizontal = true;
  bi_node_add_node(background,n);
  // Flip Horizontal (Normal)
  n = make_sprite_from_texture(tex);
  bi_node_set_position(n,360,240);
  n->_texture_flip_horizontal = true;
  bi_node_add_node(background,n);

  // Flip Vertical (Crop)
  n = make_sprite_from_texture(tex);
  bi_node_set_position(n,120,80);
  n->_texture_flip_vertical = true;
  bi_node_set_cropped_texture(n,tex, 64,32,64,96, 64,32,128,128 );
  bi_node_add_node(background,n);
  // Flip Vertical (Normal)
  n = make_sprite_from_texture(tex);
  bi_node_set_position(n,120,80);
  n->_texture_flip_vertical = true;
  bi_node_add_node(background,n);

  // Crop only
  n = make_sprite_from_texture(tex);
  bi_node_set_position(n,360,80);
  bi_node_set_cropped_texture(n,tex, 64,32,64,96, 64,32,128,128 );
  bi_node_add_node(background,n);

  // layer
  BiLayer *layer = bi_layer_init(malloc(sizeof(BiLayer)));
  bi_add_layer(context,layer);
  layer->root = background;
  layer->textures[0] = background->_texture;
  layer->textures[1] = tex;

  bi_start_run_loop(context);
  return 0;
}
