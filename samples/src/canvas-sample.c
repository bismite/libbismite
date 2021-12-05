#include "common.h"
#include "bi/canvas.h"

int main(int argc, char* argv[])
{
  BiContext* context = malloc(sizeof(BiContext));
  bi_init_context(context, 480, 320, 0, true, __FILE__);
  print_info(context);

  // layer
  BiLayer *layer = malloc(sizeof(BiLayer));
  bi_layer_init(layer);
  bi_add_layer(context,layer);
  layer->root = make_sprite_with_anchor("assets/check.png",0,0);
  layer->textures[0] = layer->root->texture_mapping->texture;

  // Canvas
  BiNode* face = make_sprite_with_anchor("assets/face01.png",0,0);
  BiNode* tile = make_sprite_with_anchor("assets/tile.png",0,0);
  BiCanvas* canvas = bi_canvas_init(ALLOC(BiCanvas),256,256);
  canvas->shader = &context->default_shader;
  canvas->textures[0] = face->texture_mapping->texture;
  canvas->textures[1] = tile->texture_mapping->texture;
  bi_set_blend_factor(&canvas->blend_factor,GL_ONE,GL_ZERO,GL_ONE,GL_ZERO);
  bi_node_add_node(face,tile);

  bi_node_set_anchor(face,0.5,0.5);
  bi_node_set_position(face,64,32);
  bi_node_set_degree(face,45);
  bi_node_set_scale(face,1,2);
  bi_set_color(face->color,0,0xff,0,0x99);

  // draw
  bi_canvas_clear(canvas,0xff,0,0,64);
  bi_canvas_draw(canvas,face);

  // save image
  bi_canvas_save_png_image(canvas,"canvas.png");

  // Sprite from Canvas
  BiNode *sprite = bi_node_init(ALLOC(BiNode));
  bi_node_set_size(sprite, canvas->w,canvas->h);
  bi_node_set_anchor(sprite,0.5,0.5);
  bi_node_set_position(sprite,context->w/2,context->h/2);
  BiTexture *texture = bi_canvas_to_texture(canvas,ALLOC(BiTexture));
  sprite->texture_mapping = bi_texture_mapping_init(ALLOC(BiTextureMapping),texture);
  layer->textures[1] = texture;
  bi_node_add_node(layer->root, sprite);

  // original
  layer->textures[2] = face->texture_mapping->texture;
  layer->textures[3] = tile->texture_mapping->texture;
  bi_node_add_node(layer->root,face);

  bi_start_run_loop(context);
  return 0;
}
