#include "common.h"

// 200x200, Blue Background
BiNode* draw_canvas(BiContext* context)
{
  BiNode* canvas = bi_node_init(ALLOC(BiNode));
  bi_node_set_size(canvas,200,200);
  canvas->framebuffer = bi_framebuffer_init(ALLOC(BiFramebuffer),canvas->w,canvas->h);
  BiNode *face = make_sprite("assets/face01.png");
  bi_node_set_position(face,180,180);
  // Shader
  BiShaderNode *shader_node = bi_shader_node_init(ALLOC(BiShaderNode));
  bi_node_add_node(shader_node, face);
  shader_node->textures[0] = face->texture;
  bi_node_add_node(canvas, shader_node);
  // Draw
  bi_framebuffer_clear(canvas->framebuffer,0,0,0xff,0x80);
  bi_draw_framebuffer_node(context,canvas);
  return canvas;
}

int main(int argc, char* argv[])
{
  BiContext* context = make_context(__FILE__);

  // Framebuffer Canvas
  BiNode* canvas = draw_canvas(context);

  // Sprite from Canvas
  BiNode *canvas_sprite = bi_node_init(ALLOC(BiNode));
  bi_node_set_size(canvas_sprite, canvas->w,canvas->h);
  bi_node_set_position(canvas_sprite, 10,10);
  canvas_sprite->texture_flip_vertical = true; // UPSIDE DOWN
  BiTexture *canvas_tex = bi_texture_init_with_framebuffer(ALLOC(BiTexture),canvas->framebuffer);
  bi_node_set_texture(canvas_sprite,canvas_tex,0,0,canvas_tex->w,canvas_tex->h);

  // Main Layer
  BiShaderNode *shader_node = bi_shader_node_init(ALLOC(BiShaderNode));
  BiNode* background = bi_node_add_node(shader_node,make_bg("assets/check.png"));
  bi_node_add_node(shader_node, canvas_sprite);
  shader_node->textures[0] = background->texture;
  shader_node->textures[1] = canvas_tex;
  bi_node_add_node(&context->default_framebuffer_node,shader_node);

  // start
  bi_start_run_loop(context);
  return 0;
}