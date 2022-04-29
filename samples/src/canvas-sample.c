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
  layer->textures[0] = layer->root->_texture;
  // Canvas
  BiNode* sprite = make_sprite_with_anchor("assets/tester.png",0,0);
  BiNode* face = make_sprite("assets/face01.png");
  BiCanvas* canvas = bi_canvas_init(ALLOC(BiCanvas),256,256);
  canvas->shader = &context->default_shader;
  canvas->textures[0] = sprite->_texture;
  canvas->textures[1] = face->_texture;

  face->opacity = 0.5;
  bi_node_add_node(sprite,face);
  bi_node_set_position(sprite,10,10);
  bi_node_set_position(face,sprite->_w/2.0,sprite->_h/2.0);
  bi_node_set_degree(face,45);
  bi_set_color(face->color,0,0xff,0,0x99);

  // draw
  bi_canvas_clear(canvas,0,0,0,0);
  bi_canvas_draw(canvas,sprite);

  // Sprite from Canvas
  BiNode *canvas_sprite = bi_node_init(ALLOC(BiNode));
  bi_node_set_size(canvas_sprite, canvas->w,canvas->h);
  bi_node_set_position(canvas_sprite,192,0);
  BiTexture *canvas_texture = bi_canvas_to_texture(canvas,ALLOC(BiTexture));
  bi_node_set_texture(canvas_sprite,canvas_texture,0,0,canvas_texture->w,canvas_texture->h);
  layer->textures[1] = canvas_texture;
  bi_node_add_node(layer->root, canvas_sprite);

  // original
  layer->textures[2] = sprite->_texture;
  layer->textures[3] = face->_texture;
  bi_node_add_node(layer->root,sprite);

  bi_start_run_loop(context);
  return 0;
}
