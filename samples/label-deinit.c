#include "common.h"

int main(int argc, char* argv[])
{
  BiContext* context = make_context(__FILE__);
  // layer
  BiLayer *layer = bi_layer_init(ALLOC(BiLayer));
  bi_add_layer(context,layer);
  BiNode* root = bi_layer_add_node(layer,make_bg("assets/map.png"));
  layer->textures[1] = root->texture;
  // font
  BiTexture *font_texture = bi_texture_init_with_filename(ALLOC(BiTexture),"assets/font.png",false);
  layer->textures[0] = font_texture;
  BiFont *font = load_font("assets/font14b.dat",font_texture);
  // label
  BiLabel* label = bi_label_init( ALLOC(BiLabel), font );
  bi_label_set_background_color( label, RGBA(0xff,0xff,0xff,0xff) );
  bi_label_set_modulate_color(label, RGBA(0,0,0,0xff) );
  bi_label_set_text(label, "The quick brown fox jumps over the lazy dog" );
  bi_label_set_color_with_range( label, 1,4, RGBA(0xff,0,0,0xff), RGBA(0xff,0xff,0xff,0xff) );
  bi_node_set_position( (BiNode*)label, 100, 100 );
  bi_node_add_node(root,(BiNode*)label);
  // remove and deinit
  bi_node_remove_node(root,(BiNode*)label);
  bi_label_deinit(label);
  free(label);
  label = NULL;

  // start
  bi_start_run_loop(context);
  return 0;
}
