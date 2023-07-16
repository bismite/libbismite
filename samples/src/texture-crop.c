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
  bi_node_set_position(n,100,240);
  bi_node_add_node(background,n);
  // Crop
  n = make_sprite_from_texture(tex);
  bi_node_set_position(n,180,240);
  bi_node_set_cropped_texture(n,tex, 64,32,64,96, 64,32,128,128 );
  bi_node_add_node(background,n);

  // Flip Horizontal (Normal)
  n = make_sprite_from_texture(tex);
  bi_node_set_position(n,320,240);
  n->texture_flip_horizontal = true;
  bi_node_add_node(background,n);
  // Flip Horizontal and Crop
  n = make_sprite_from_texture(tex);
  bi_node_set_position(n,460,240);
  bi_node_set_cropped_texture(n,tex, 64,32,64,96, 64,32,128,128 );
  n->texture_flip_horizontal = true;
  bi_node_add_node(background,n);

  // Flip Vertical (Normal)
  n = make_sprite_from_texture(tex);
  bi_node_set_position(n,100,80);
  n->texture_flip_vertical = true;
  bi_node_add_node(background,n);
  // Flip Vertical and Crop
  n = make_sprite_from_texture(tex);
  bi_node_set_position(n,180,80);
  n->texture_flip_vertical = true;
  bi_node_set_cropped_texture(n,tex, 64,32,64,96, 64,32,128,128 );
  bi_node_add_node(background,n);

  // Flip Vertical and Horizontal (Normal)
  n = make_sprite_from_texture(tex);
  bi_node_set_position(n,320,80);
  n->texture_flip_vertical = true;
  n->texture_flip_horizontal = true;
  bi_node_add_node(background,n);
  // Crop and Flip V&H
  n = make_sprite_from_texture(tex);
  bi_node_set_position(n,460,80);
  n->texture_flip_vertical = true;
  n->texture_flip_horizontal = true;
  bi_node_set_cropped_texture(n,tex, 64,32,64,96, 64,32,128,128 );
  bi_node_add_node(background,n);

  // layer
  BiLayer *layer = bi_layer_init(ALLOC(BiLayer));
  bi_add_layer(context,layer);
  layer->root = background;
  layer->textures[0] = background->texture;
  layer->textures[1] = tex;

  bi_start_run_loop(context);
  return 0;
}
