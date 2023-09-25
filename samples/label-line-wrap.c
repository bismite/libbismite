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
  BiContext* context = make_context(__FILE__);

  // layer
  BiLayer *layer = bi_layer_init(ALLOC(BiLayer));
  bi_add_layer(context,layer);
  BiNode* root = bi_layer_add_node(layer, make_bg("assets/check.png"));
  layer->textures[1] = root->texture;

  // bounds
  BiNode* bounds = bi_node_init(ALLOC(BiNode));
  bi_node_set_size(bounds,WIDTH,320);
  bi_node_set_position(bounds,20,0);
  bounds->color_tint = RGBA32(0x006666ff);
  bi_node_add_node(root, bounds);

  // font
  BiTexture *font_texture = bi_texture_init_with_filename(ALLOC(BiTexture),"assets/font.png",false);
  BiFont *font = load_font("assets/font14.dat", font_texture);
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
    BiLabel* label = bi_label_init(ALLOC(BiLabel),font);
    bi_label_set_background_color(label,RGBA(0x33,0,0,0xff));
    bi_label_set_text(label,lines[i]);
    bi_node_set_position( (BiNode*)label, 20, 320-40-i*20 );
    bi_node_add_node(root,(BiNode*)label);
  }

  bi_start_run_loop(context);
  return 0;
}
