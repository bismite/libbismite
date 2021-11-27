#include "common.h"

#define SIZE 8

int main(int argc, char* argv[])
{
  BiContext* context = bi_init_context(malloc(sizeof(BiContext)), 480, 320, 0, true, __FILE__);
  print_info(context);

  // Background layer
  BiLayer *bg = bi_layer_init(malloc(sizeof(BiLayer)));
  bi_add_layer(context,bg);
  bg->root = make_sprite_with_anchor("assets/map.png",0,0);
  bg->textures[0] = bg->root->texture_mapping->texture;

  //
  struct Blend{ GLuint src; GLuint dst; GLuint asrc; GLuint adst; };
  struct Blend b[SIZE] = {
    { GL_SRC_ALPHA,           GL_ONE_MINUS_SRC_ALPHA }, // Normal
    { GL_SRC_ALPHA,           GL_ONE },                 // Additive
    { GL_ONE,                 GL_ONE },                 // Additive2
    { GL_ONE_MINUS_DST_COLOR, GL_ONE },                 // Additive3 Screen
    { GL_ZERO,                GL_SRC_COLOR },           // Multiply
    { GL_ONE_MINUS_DST_COLOR, GL_ZERO },                // Negative
    { GL_DST_COLOR,           GL_ONE },                 // spotlight
    { GL_ONE_MINUS_DST_COLOR, GL_ONE_MINUS_SRC_COLOR}   // xor
  };
  const int W=4, H=2, IW=108,IH=108;
  for(int i=0;i<SIZE;i++) {
    BiLayer *a = bi_layer_init(malloc(sizeof(BiLayer)));
    bi_add_layer(context,a);
    a->root = make_sprite_with_anchor("assets/tester.png",0,0);
    bi_node_set_scale(a->root, (float)IW/a->root->w, (float)IH/a->root->h );
    bi_node_set_position(a->root, (context->w-IW*W)/(W+1)*(1+i%W)+IW*(i%W), (context->h-IH*H)/(H+1)*(1+i/W)+IH*(i/W));
    a->textures[0] = a->root->texture_mapping->texture;
    a->blend_factor.src = b[i].src;
    a->blend_factor.dst = b[i].dst;
  }

  bi_start_run_loop(context);
  return 0;
}
