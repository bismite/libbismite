#include "common.h"

BiNode* draw_canvas(BiContext* context)
{
  BiNode* canvas = bi_node_init(ALLOC(BiNode));
  bi_node_set_size(canvas,context->w,context->h);
  canvas->framebuffer = bi_framebuffer_init(ALLOC(BiFramebuffer),canvas->w,canvas->h);

  // Straight Alpha / Premultiplied Alpha
  BiTexture* tex_s = bi_texture_init_with_filename(ALLOC(BiTexture),"assets/face01.png",true);
  BiTexture* tex_p = bi_texture_init_with_filename(ALLOC(BiTexture),"assets/face01.png",false);
  BiNode *face_s = make_sprite_from_texture(tex_s);
  BiNode *face_p = make_sprite_from_texture(tex_p);
  bi_node_set_position(face_s,100,100);
  bi_node_set_position(face_p,200,100);
  // Layer
  BiShaderNode *shader_node = bi_shader_node_init(ALLOC(BiShaderNode));
  shader_node->blend_factor = bi_blend_factor(GL_ONE,GL_ZERO,GL_ONE,GL_ZERO); // Overwrite
  bi_node_add_node(shader_node, face_s);
  bi_node_add_node(shader_node, face_p);
  shader_node->textures[0] = tex_s;
  shader_node->textures[1] = tex_p;
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
  // Save Image
  // UPSIDE DOWN, Premultiplied-Alpha Texture are Darked.
  bi_framebuffer_save_png_image(canvas->framebuffer, "framebuffer-straight-alpha.png" );

  // Sprite from Canvas
  BiNode *canvas_sprite = bi_node_init(ALLOC(BiNode));
  bi_node_set_size(canvas_sprite, canvas->w,canvas->h);
  canvas_sprite->texture_flip_vertical = true; // UPSIDE DOWN
  BiTexture *canvas_tex = bi_texture_init_with_framebuffer(ALLOC(BiTexture),canvas->framebuffer);
  bi_node_set_texture(canvas_sprite,canvas_tex,0,0,canvas_tex->w,canvas_tex->h);

  // Main Layer
  BiShaderNode *shader_node = bi_shader_node_init(ALLOC(BiShaderNode));
  BiNode* background = bi_node_add_node(shader_node,make_bg("assets/check.png"));
  BiNode* face = make_sprite("assets/face01.png");
  bi_node_set_position(face,150,240);
  bi_node_add_node(shader_node, canvas_sprite);
  bi_node_add_node(shader_node, face);
  shader_node->textures[0] = bi_node_get_texture(background);
  shader_node->textures[1] = bi_node_get_texture(face);
  shader_node->textures[2] = canvas_tex;
  bi_node_add_node(&context->default_framebuffer_node,shader_node);

  // start
  bi_start_run_loop(context);
  return 0;
}
