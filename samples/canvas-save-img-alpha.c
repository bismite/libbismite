#include "common.h"

BiFramebufferNode* draw_canvas(BiContext* context)
{
  BiFramebufferNode* canvas = bi_framebuffer_node_init_with_size(ALLOC(BiFramebufferNode),context->w,context->h);
  // Straight Alpha / Premultiplied Alpha
  BiTexture* tex_s = bi_texture_init_with_filename(ALLOC(BiTexture),"assets/face01.png",true);
  BiTexture* tex_p = bi_texture_init_with_filename(ALLOC(BiTexture),"assets/face01.png",false);
  BiNode *face_s = make_sprite_from_texture(tex_s);
  BiNode *face_p = make_sprite_from_texture(tex_p);
  bi_node_set_position(face_s,100,100);
  bi_node_set_position(face_p,200,100);
  // Layer
  BiLayer *layer = bi_layer_init(ALLOC(BiLayer));
  layer->blend_factor = bi_blend_factor(GL_ONE,GL_ZERO,GL_ONE,GL_ZERO); // Overwrite
  bi_layer_add_node(layer, face_s);
  bi_layer_add_node(layer, face_p);
  layer->textures[0] = tex_s;
  layer->textures[1] = tex_p;
  bi_framebuffer_node_add_layer(canvas, layer);
  // Draw
  bi_framebuffer_node_clear(canvas,0,0,0,0);
  bi_framebuffer_node_draw(canvas,context);
  return canvas;
}

int main(int argc, char* argv[])
{
  BiContext* context = make_context(__FILE__);

  // Canvas
  BiFramebufferNode* canvas = draw_canvas(context);
  // Save Image
  // UPSIDE DOWN, Premultiplied-Alpha Texture are Darked.
  bi_framebuffer_save_png_image(&canvas->framebuffer,"canvas-save-img-alpha.png");

  // Sprite from Canvas
  BiNode *canvas_sprite = bi_node_init(ALLOC(BiNode));
  bi_node_set_size(canvas_sprite, canvas->w,canvas->h);
  canvas_sprite->texture_flip_vertical = true; // UPSIDE DOWN
  BiTexture *canvas_tex = bi_texture_init_with_framebuffer(ALLOC(BiTexture),&canvas->framebuffer);
  bi_node_set_texture(canvas_sprite,canvas_tex,0,0,canvas_tex->w,canvas_tex->h);

  // Main Layer
  BiLayer *layer = bi_layer_init(ALLOC(BiLayer));
  BiNode* background = bi_layer_add_node(layer,make_bg("assets/check.png"));
  BiNode* face = make_sprite("assets/face01.png");
  bi_node_set_position(face,150,240);
  bi_layer_add_node(layer, canvas_sprite);
  bi_layer_add_node(layer, face);
  layer->textures[0] = background->texture;
  layer->textures[1] = face->texture;
  layer->textures[2] = canvas_tex;
  bi_add_layer(context,layer);

  // start
  bi_start_run_loop(context);
  return 0;
}
