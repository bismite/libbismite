#include "common.h"

int main(int argc, char* argv[])
{
  BiContext* context = bi_init_context(ALLOC(BiContext), 480, 320, 0, false, __FILE__);
  print_info(context);

  // layer
  BiLayer *layer = bi_layer_init(ALLOC(BiLayer));
  bi_add_layer(context,layer);

  // root node
  BiNode* root = bi_node_init(ALLOC(BiNode));
  layer->root = root;

  // font
  BiTexture *font_texture = malloc(sizeof(BiTexture));
  bi_texture_init_with_filename(font_texture,"assets/font.png",false);
  layer->textures[0] = font_texture;
  const char* fonts[4] = {
    "assets/font12.dat", "assets/font12b.dat", "assets/font14.dat", "assets/font14b.dat"
  };
  // labels
  int offset_x = 10;
  int offset_y = 10;
  int line=18;
  for(int i=0; i<4; i++){
    BiLabel* label = bi_label_init( ALLOC(BiLabel), load_font_atlas(fonts[i],font_texture) );
    bi_label_set_background_color( label, RGBA(0xff,0xff,0xff,0xff) );
    bi_label_set_modulate_color(label, RGBA(0,0,0,0xff) );
    bi_label_set_text(label, "The quick brown fox jumps over the lazy dog" );
    bi_label_set_color_with_range( label, 1,4, RGBA(0xff,0,0,0xff), RGBA(0xff,0xff,0xff,0xff) );
    bi_node_set_position( (BiNode*)label, offset_x, offset_y+i*line );
    bi_node_add_node(root,(BiNode*)label);
  }
  for(int i=0; i<4; i++){
    BiLabel* label = bi_label_init( ALLOC(BiLabel), load_font_atlas(fonts[i],font_texture) );
    bi_label_set_background_color( label, RGBA(0,0xff,0,0xff) );
    bi_label_set_modulate_color(label, RGBA(0x64,0x64,0x64,0xFF) );
    bi_label_set_text(label, "いろはにほへと　ちりぬるを　わかよたれそ　つねならむ" );
    bi_label_set_color_with_range( label, 1,4, RGBA(0xff,0,0,0xff), RGBA(0xff,0xff,0xff,0xff) );
    bi_node_set_position( (BiNode*)label, offset_x, offset_y+(4+i)*line );
    bi_node_add_node(root,(BiNode*)label);
  }
  for(int i=0; i<4; i++){
    BiLabel* label = bi_label_init( ALLOC(BiLabel), load_font_atlas(fonts[i],font_texture) );
    bi_label_set_background_color( label, RGBA(32,32,32,0xff) );
    bi_label_set_tint_color(label, RGBA(0,0xff,0xff,0xFF) );
    bi_label_set_text(label, "カタカナと、Alphabetと、ひらがな。" );
    bi_label_set_color_with_range( label, 1,4, RGBA(0xff,0,0,0xff), RGBA(0xff,0xff,0xff,0xff) );
    bi_node_set_position( (BiNode*)label, offset_x, offset_y+(8+i)*line );
    bi_node_add_node(root,(BiNode*)label);
  }
  for(int i=0; i<4; i++){
    BiLabel* label = bi_label_init( ALLOC(BiLabel), load_font_atlas(fonts[i],font_texture) );
    bi_label_set_background_color( label, RGBA(0x33,0,0,0xff) );
    bi_label_set_tint_color(label, RGBA(0xff,0,0xff,0xFF) );
    bi_label_set_text(label, "0123456789 !\"#$%&'()*+,-./:;<=>?@[\\]^_`{|}~" );
    bi_label_set_color_with_range( label, 1,4, RGBA(0xff,0,0,0xff), RGBA(0xff,0xff,0xff,0xff) );
    bi_node_set_position( (BiNode*)label, offset_x, offset_y+(12+i)*line );
    bi_node_add_node(root,(BiNode*)label);
  }

  // fps layer
  add_fps_layer(context, load_font_atlas(fonts[0],font_texture) );

  bi_start_run_loop(context);
  return 0;
}