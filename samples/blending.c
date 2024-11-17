#include "common.h"

#define SIZE 9

int main(int argc, char* argv[])
{
  BiContext* context = make_context(__FILE__);

  // Background shader_node
  BiShaderNode *bg = bi_shader_node_init(ALLOC(BiShaderNode));
  bg->textures[0] = bi_node_get_texture(bi_node_add_node(bg,make_bg("assets/map.png")));
  bi_node_add_node(&context->default_framebuffer_node,bg);

  //
  struct Blend{ GLuint src; GLuint dst; GLuint asrc; GLuint adst; };
  struct Blend b[SIZE] = {
    { GL_SRC_ALPHA,           GL_ONE_MINUS_SRC_ALPHA }, // Normal
    { GL_ONE,                 GL_ONE_MINUS_SRC_ALPHA }, // Premultiplied Alpha
    { GL_ZERO,                GL_SRC_COLOR },           // Multiply
    { GL_ONE_MINUS_DST_COLOR, GL_ZERO },                // Negative
    // -
    { GL_SRC_ALPHA,           GL_ONE },                 // Additive
    { GL_ONE,                 GL_ONE },                 // Additive2
    { GL_ONE_MINUS_DST_COLOR, GL_ONE },                 // Additive3
    { GL_DST_COLOR,           GL_ONE },                 // spotlight
    // -
    { GL_ONE_MINUS_DST_COLOR, GL_ONE_MINUS_SRC_COLOR}   // xor
  };
  const int COL=4, ROW=3, CW=96,RH=96;
  for(int i=0;i<SIZE;i++) {
    BiShaderNode *a = bi_shader_node_init(ALLOC(BiShaderNode));
    bi_node_add_node(&context->default_framebuffer_node,a);
    BiNode *n = bi_node_add_node(a,make_sprite_with_anchor("assets/tester.png",0,0) );
    bi_node_set_scale(n, (float)CW/n->w, (float)RH/n->h );
    int x = (context->w-CW*COL)/(COL+1)*(1+i%COL)+CW*(i%COL);
    int y = (context->h-RH*ROW)/(ROW+1)*(1+i/COL)+RH*(i/COL);
    bi_node_set_position(n,x,y);
    a->textures[0] = bi_node_get_texture(n);
    a->blend_factor.src = b[i].src;
    a->blend_factor.dst = b[i].dst;
  }

  bi_start_run_loop(context);
  return 0;
}
