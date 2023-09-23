#include "common.h"
#include "bi/canvas.h"

int main(int argc, char* argv[])
{
  BiContext* context = make_context(__FILE__);

  // Straight Alpha Texture
  BiTexture* tex = MAKE_TEXTURE("assets/tester.png");
  BiNode *sprite = make_sprite_from_texture(tex);
  bi_node_set_anchor(sprite,0,0);

  // Canvas
  BiCanvas* canvas = bi_canvas_init(ALLOC(BiCanvas),128,128);
  canvas->shader = &context->default_shader;
  canvas->textures[0] = tex;
  bi_set_blend_factor(&canvas->blend_factor,GL_ONE,GL_ZERO,GL_ONE,GL_ZERO);
  // draw
  bi_canvas_clear(canvas,0,0,0,0);
  bi_canvas_draw(canvas,sprite);
  // save image
  bi_canvas_save_png_image(canvas,"canvas.png");

  //
  // Show Canvas
  //

  // Layer
  BiLayer *layer = bi_layer_init(ALLOC(BiLayer));
  bi_add_layer(context,layer);
  BiNode* root = bi_layer_add_node(layer,make_bg("assets/map.png"));
  layer->textures[0] = root->texture;
  // straight blending
  bi_set_blend_factor(&layer->blend_factor,GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA,GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

  // Sprite from Canvas
  BiNode *canvas_sprite = bi_node_init(ALLOC(BiNode));
  bi_node_set_size(canvas_sprite, canvas->w,canvas->h);
  bi_node_set_anchor(canvas_sprite,0.5,0.5);
  bi_node_set_position(canvas_sprite,context->w/2,context->h/2);
  BiTexture *canvas_texture = bi_canvas_to_texture(canvas,ALLOC(BiTexture));
  bi_node_set_texture(canvas_sprite,canvas_texture,0,0,canvas_texture->w,canvas_texture->h);
  layer->textures[1] = canvas_texture;
  bi_node_add_node(root, canvas_sprite);

  bi_start_run_loop(context);
  return 0;
}
