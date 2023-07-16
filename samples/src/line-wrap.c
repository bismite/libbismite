#include "common.h"

#define WIDTH 120*2

int copy_a_char(char* dst, char* src)
{
  if((unsigned char)src[0] >= 0b11110000){
    dst[0] = src[0];
    dst[1] = src[1];
    dst[2] = src[2];
    dst[3] = src[3];
    return 4;
  }else if((unsigned char)src[0] >= 0b11100000){
    dst[0] = src[0];
    dst[1] = src[1];
    dst[2] = src[2];
    return 3;
  }else if((unsigned char)src[0] >= 0b11000000){
    dst[0] = src[0];
    dst[1] = src[1];
    return 2;
  }
  dst[0] = src[0];
  return 1;
}

int copy_utf8_str(char* buf, char* text, int num)
{
  int total=0;
  int bytes=0;
  for(int i=0;i<num;i++){
    bytes = copy_a_char(buf,text);
    buf+=bytes;
    text+=bytes;
    total += bytes;
  }
  return total;
}

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

  // bounds
  BiNode* bounds = bi_node_init(ALLOC(BiNode));
  bi_node_set_size(bounds,WIDTH,320);
  bi_node_set_position(bounds,20,0);
  bi_set_color(bounds->color_tint,0,0x66,0x66,0xff);
  bi_node_add_node(layer->root, bounds);

  // font
  BiTexture *font_texture = ALLOC(BiTexture);
  bi_texture_init_with_filename(font_texture,"assets/font.png",false);
  BiFontAtlas *font = load_font_atlas("assets/font14.dat", font_texture);
  layer->textures[0] = font_texture;

  // txt
  char* text =
    "The quick brown fox jumps over the lazy dog"
    "いろはにほへと　ちりぬるを　わかよたれそ　つねならむ"
    "カタカナと、Alphabetと、ひらがな。"
    "0123456789 !\"#$%&'()*+,-./:;<=>?@[\\]^_`{|}~";
  // break
  char* tmp = text;
  char* lines[32];
  int line_count=0;
  while( *tmp != 0 ){
    int num = bi_font_line_x_to_index(font,tmp,WIDTH);
    char *buf = malloc(num*4+1);
    int bytes = copy_utf8_str(buf,tmp,num);
    buf[bytes] = 0;
    printf("[num=%d][bytes=%d] %s\n",num,bytes,buf);
    tmp += bytes;
    lines[line_count] = buf;
    line_count++;
  }

  // label
  for(int i=0;i<line_count;i++){
    BiNode* label = bi_node_init(ALLOC(BiNode));
    bi_set_color(label->color_tint,0x33,0,0,0xff);
    bi_node_set_position( label, 20, 320-40-i*20 );
    bi_label_set_text(label,font,lines[i]);
    bi_node_add_node(root,label);
  }

  bi_start_run_loop(context);
  return 0;
}
