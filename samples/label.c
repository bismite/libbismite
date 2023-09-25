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
  const char* fonts[4] = {
    "assets/font12.dat", "assets/font12b.dat", "assets/font14.dat", "assets/font14b.dat"
  };
  // labels
  int offset_x = 10;
  int offset_y = 10;
  int line=18;
  for(int i=0; i<4; i++){
    BiLabel* label = bi_label_init( ALLOC(BiLabel), load_font(fonts[i],font_texture) );
    bi_label_set_background_color( label, RGBA(0xff,0xff,0xff,0xff) );
    bi_label_set_modulate_color(label, RGBA(0,0,0,0xff) );
    bi_label_set_text(label, "The quick brown fox jumps over the lazy dog" );
    bi_label_set_color_with_range( label, 1,4, RGBA(0xff,0,0,0xff), RGBA(0xff,0xff,0xff,0xff) );
    bi_node_set_position( (BiNode*)label, offset_x, offset_y+i*line );
    bi_node_add_node(root,(BiNode*)label);
  }
  for(int i=0; i<4; i++){
    BiLabel* label = bi_label_init( ALLOC(BiLabel), load_font(fonts[i],font_texture) );
    bi_label_set_background_color( label, RGBA(0,0xff,0,0xff) );
    bi_label_set_modulate_color(label, RGBA(0x64,0x64,0x64,0xFF) );
    bi_label_set_text(label, "いろはにほへと　ちりぬるを　わかよたれそ　つねならむ" );
    bi_label_set_color_with_range( label, 1,4, RGBA(0xff,0,0,0xff), RGBA(0xff,0xff,0xff,0xff) );
    bi_node_set_position( (BiNode*)label, offset_x, offset_y+(4+i)*line );
    bi_node_add_node(root,(BiNode*)label);
  }
  for(int i=0; i<4; i++){
    BiLabel* label = bi_label_init( ALLOC(BiLabel), load_font(fonts[i],font_texture) );
    bi_label_set_background_color( label, RGBA(32,32,32,0xff) );
    bi_label_set_tint_color(label, RGBA(0,0xff,0xff,0xFF) );
    bi_label_set_text(label, "カタカナと、Alphabetと、ひらがな。" );
    bi_label_set_color_with_range( label, 1,4, RGBA(0xff,0,0,0xff), RGBA(0xff,0xff,0xff,0xff) );
    bi_node_set_position( (BiNode*)label, offset_x, offset_y+(8+i)*line );
    bi_node_add_node(root,(BiNode*)label);
  }
  for(int i=0; i<4; i++){
    BiLabel* label = bi_label_init( ALLOC(BiLabel), load_font(fonts[i],font_texture) );
    bi_label_set_background_color( label, RGBA(0x33,0,0,0xff) );
    bi_label_set_tint_color(label, RGBA(0xff,0,0xff,0xFF) );
    bi_label_set_text(label, "0123456789 !\"#$%&'()*+,-./:;<=>?@[\\]^_`{|}~" );
    bi_label_set_color_with_range( label, 1,4, RGBA(0xff,0,0,0xff), RGBA(0xff,0xff,0xff,0xff) );
    bi_node_set_position( (BiNode*)label, offset_x, offset_y+(12+i)*line );
    bi_node_add_node(root,(BiNode*)label);
  }
  // start
  bi_start_run_loop(context);
  return 0;
}
