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
  BiFontAtlas *fonts[4];
  fonts[0] = load_font_atlas("assets/font12.dat", font_texture),
  fonts[1] = load_font_atlas("assets/font12b.dat", font_texture);
  fonts[2] = load_font_atlas("assets/font14.dat", font_texture);
  fonts[3] = load_font_atlas("assets/font14b.dat", font_texture);
  layer->textures[0] = font_texture;

  // labels
  int offset_x = 10;
  int offset_y = 10;
  int line=18;
  for(int i=0; i<4; i++){
    BiNode* label = malloc(sizeof(BiNode));
    bi_node_init(label);
    bi_set_color(label->color,0xff,0xff,0xff,0xff);
    bi_node_set_position( label, offset_x, offset_y+i*line );
    bi_update_label(label, "The quick brown fox jumps over the lazy dog", fonts[i], 0,0,0,0xFF);
    bi_node_add_node(root,label);
  }
  for(int i=0; i<4; i++){
    BiNode* label = malloc(sizeof(BiNode));
    bi_node_init(label);
    bi_set_color(label->color,0x64,0x64,0x64,0xcc);
    bi_node_set_position( label, offset_x, offset_y+(4+i)*line );
    bi_update_label(label, "いろはにほへと　ちりぬるを　わかよたれそ　つねならむ", fonts[i], 0xFF,0xFF,0xFF,0xFF);
    bi_node_add_node(root,label);
  }
  for(int i=0; i<4; i++){
    BiNode* label = malloc(sizeof(BiNode));
    bi_node_init(label);
    bi_set_color(label->color,32,32,32,0xff);
    bi_node_set_position( label, offset_x, offset_y+(8+i)*line );
    bi_update_label(label, "カタカナと、Alphabetと、ひらがな。", fonts[i], 0xFF,0xFF,0xFF,0xFF);
    bi_node_add_node(root,label);
  }
  for(int i=0; i<4; i++){
    BiNode* label = malloc(sizeof(BiNode));
    bi_node_init(label);
    bi_set_color(label->color,32,32,32,0xff);
    bi_node_set_position( label, offset_x, offset_y+(12+i)*line );
    bi_update_label(label, "0123456789 !\"#$%&'()*+,-./:;<=>?@[\\]^_`{|}~", fonts[i], 0xFF,0xFF,0xFF,0xFF);
    bi_node_add_node(root,label);
  }

  // fps layer
  add_fps_layer(context,fonts[0]);

  bi_start_run_loop(context);
  return 0;
}
