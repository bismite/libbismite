#include "common.h"
#include "bi/canvas.h"

int main(int argc, char* argv[])
{
  BiContext* context = malloc(sizeof(BiContext));
  bi_init_context(context, 480, 320, 0, false, __FILE__);
  print_info(context);

  // Straight Alpha Texture
  BiTexture* tex = bi_texture_init_with_filename(ALLOC(BiTexture),"assets/tester.png",true);
  BiTextureMapping *mapping = bi_texture_mapping_init(ALLOC(BiTextureMapping),tex);
  BiNode *sprite = make_sprite_from_mapping(mapping);
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
  BiLayer *layer = malloc(sizeof(BiLayer));
  bi_layer_init(layer);
  bi_add_layer(context,layer);
  layer->root = make_sprite_with_anchor("assets/map.png",0,0);
  layer->textures[0] = layer->root->texture_mapping->texture;
  // straight blending
  bi_set_blend_factor(&layer->blend_factor,GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA,GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

  // Sprite from Canvas
  BiNode *canvas_sprite = bi_node_init(ALLOC(BiNode));
  bi_node_set_size(canvas_sprite, canvas->w,canvas->h);
  bi_node_set_anchor(canvas_sprite,0.5,0.5);
  bi_node_set_position(canvas_sprite,context->w/2,context->h/2);
  BiTexture *canvas_texture = bi_canvas_to_texture(canvas,ALLOC(BiTexture));
  canvas_sprite->texture_mapping = bi_texture_mapping_init(ALLOC(BiTextureMapping),canvas_texture);
  layer->textures[1] = canvas_texture;
  bi_node_add_node(layer->root, canvas_sprite);

  bi_start_run_loop(context);
  return 0;
}
