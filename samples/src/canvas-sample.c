#include "common.h"
#include "bi/canvas.h"

int main(int argc, char* argv[])
{
  BiContext* context = malloc(sizeof(BiContext));
  bi_init_context(context, 480, 320, 0, false, __FILE__);
  print_info(context);

  // layer
  BiLayer *layer = malloc(sizeof(BiLayer));
  bi_layer_init(layer);
  bi_add_layer(context,layer);
  layer->root = make_sprite_with_anchor("assets/check.png",0,0);
  layer->textures[0] = layer->root->texture_mapping->texture;
  // Canvas
  BiNode* sprite = make_sprite_with_anchor("assets/tester.png",0,0);
  BiNode* face = make_sprite("assets/face01.png");
  BiCanvas* canvas = bi_canvas_init(ALLOC(BiCanvas),256,256,context->default_texture);
  canvas->shader = &context->default_shader;
  canvas->textures[0] = sprite->texture_mapping->texture;
  canvas->textures[1] = face->texture_mapping->texture;

  face->opacity = 0.5;
  bi_node_add_node(sprite,face);
  bi_node_set_position(sprite,10,10);
  bi_node_set_position(face,sprite->w/2.0,sprite->h/2.0);
  bi_node_set_degree(face,45);
  bi_set_color(face->color,0,0xff,0,0x99);

  // draw
  bi_canvas_clear(canvas,0xff,0,0,64);
  bi_canvas_clear(canvas,0,0,0,0);
  bi_canvas_draw(canvas,sprite);

  // save image
  bi_canvas_save_png_image(canvas,"canvas.png");

  // Sprite from Canvas
  BiNode *canvas_sprite = bi_node_init(ALLOC(BiNode));
  bi_node_set_size(canvas_sprite, canvas->w,canvas->h);
  bi_node_set_position(canvas_sprite,192,0);
  BiTexture *canvas_texture = bi_canvas_to_texture(canvas,ALLOC(BiTexture));
  canvas_sprite->texture_mapping = bi_texture_mapping_init(ALLOC(BiTextureMapping),canvas_texture);
  layer->textures[1] = canvas_texture;
  bi_node_add_node(layer->root, canvas_sprite);

  // original
  layer->textures[2] = sprite->texture_mapping->texture;
  layer->textures[3] = face->texture_mapping->texture;
  bi_node_add_node(layer->root,sprite);

  bi_start_run_loop(context);
  return 0;
}
