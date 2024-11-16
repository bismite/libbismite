#include "common.h"

static const int CANVAS_W=1024;
static const int CANVAS_H=1024;

BiNode* draw_canvas(BiContext* context)
{
  BiNode* canvas = bi_node_init(ALLOC(BiNode));
  bi_node_set_size(canvas,CANVAS_W,CANVAS_H);
  canvas->framebuffer = bi_framebuffer_init(ALLOC(BiFramebuffer),canvas->w,canvas->h);

  BiTexture *tex = bi_texture_init_with_filename(ALLOC(BiTexture),"assets/face01.png",false);
  BiNode* face = bi_node_init(ALLOC(BiNode));
  bi_node_set_size(face,canvas->w,canvas->h); // fill canvas
  bi_node_set_texture(face,tex,0,0,tex->w,tex->h);

  // Shader
  BiShaderNode *shader_node = bi_shader_node_init(ALLOC(BiShaderNode));
  bi_node_add_node(shader_node, face);
  shader_node->textures[0] = face->texture;
  bi_node_add_node(canvas, shader_node);
  // Draw
  bi_framebuffer_clear(canvas->framebuffer,0,0,0,0);
  bi_draw_framebuffer_node(context,canvas);
  return canvas;
}

int main(int argc, char* argv[])
{
  BiContext* context = make_context(__FILE__);

  // Canvas
  BiNode* canvas = draw_canvas(context);
  bi_framebuffer_save_png_image(canvas->framebuffer,"framebuffer-large.png");

  // Sprite from Canvas
  BiNode *canvas_sprite = bi_node_init(ALLOC(BiNode));
  bi_node_set_size(canvas_sprite, canvas->w,canvas->h);
  bi_node_set_anchor(canvas_sprite, 0.5,0.5);
  bi_node_set_position(canvas_sprite, W/2,H/2);
  canvas_sprite->texture_flip_vertical = true; // UPSIDE DOWN
  BiTexture *canvas_tex = bi_texture_init_with_framebuffer(ALLOC(BiTexture),canvas->framebuffer);
  bi_node_set_texture(canvas_sprite,canvas_tex,0,0,canvas_tex->w,canvas_tex->h);

  // Main Layer
  BiShaderNode *shader_node = bi_shader_node_init(ALLOC(BiShaderNode));
  BiNode* background = bi_node_add_node(shader_node,make_bg("assets/check.png"));
  BiNode* face = make_sprite("assets/face01.png");
  bi_node_set_position(face,W/2,H/2);
  bi_node_add_node(shader_node, canvas_sprite);
  bi_node_add_node(shader_node, face);
  shader_node->textures[0] = background->texture;
  shader_node->textures[1] = face->texture;
  shader_node->textures[2] = canvas_tex;
  bi_node_add_node(&context->default_framebuffer_node,shader_node);

  // start
  bi_start_run_loop(context);
  return 0;
}
