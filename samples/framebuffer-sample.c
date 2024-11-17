#include "common.h"

BiNode* fbnode = NULL;

// Default Framebuffer Node
// - ShaderNode A
//   - Sprite(Check)
//   - Sprite(tester)
//   - Rect A(Red)
//   - Framebuffer Node
//     - ShaderNode B
//       - Sprite(Face)
//       - Rect B(Green)
int main(int argc, char* argv[])
{
  BiContext* context = make_context(__FILE__);

  // ShaderNode A
  BiShaderNode *shader_a = bi_shader_node_init(ALLOC(BiShaderNode));
  bi_node_add_node(&context->default_framebuffer_node,shader_a);
  // Sprites for ShaderNode A
  BiNode *bg = make_bg("assets/check.png");
  BiNode* tester = make_sprite("assets/tester.png");
  shader_a->textures[0] = bi_node_get_texture(bg);
  shader_a->textures[1] = bi_node_get_texture(tester);
  bi_node_set_position(tester,480/4,320*3/4);
  bi_node_add_node(shader_a,bg);
  bi_node_add_node(bg,tester);
  // Rectangle A (Red)
  BiNode* rect_a = bi_node_init(ALLOC(BiNode));
  rect_a->color = RGBA(0xff,0,0,128);
  bi_node_set_size(rect_a,100,100);
  bi_node_set_position(rect_a,480/4,50);
  bi_node_add_node(bg,rect_a);

  // Framebuffer Node
  fbnode = bi_node_init(ALLOC(BiNode));
  fbnode->framebuffer = bi_framebuffer_init(ALLOC(BiFramebuffer),context->w,context->h);
  fbnode->texture_flip_vertical = true;
  BiTexture* fbtex = bi_texture_init_with_framebuffer(ALLOC(BiTexture),fbnode->framebuffer);
  shader_a->textures[2] = fbtex;
  bi_node_set_texture(fbnode,fbtex,0,0,fbtex->w,fbtex->h);
  bi_node_set_size(fbnode,context->w,context->h);
  bi_node_add_node(shader_a,fbnode);

  // ShaderNode B
  BiShaderNode* shader_b = bi_shader_node_init(ALLOC(BiShaderNode));
  bi_node_add_node(fbnode,shader_b);

  // Sprite for ShaderNode B
  BiNode* face = bi_node_add_node(shader_b,make_sprite("assets/face01.png"));
  shader_b->textures[0] = bi_node_get_texture(face);
  bi_node_set_position(face,480/4*3,320*3/4);
  bi_node_add_node(shader_b,face);

  // Rectangle B (Green)
  BiNode* rect_b = bi_node_init(ALLOC(BiNode));
  rect_b->color = RGBA(0,0xff,0,128);
  bi_node_set_size(rect_b,100,100);
  bi_node_set_position(rect_b,480/4*3,50);
  bi_node_add_node(shader_b,rect_b);

  //
  bi_start_run_loop(context);
  return 0;
}
