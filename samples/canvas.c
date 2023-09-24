#include "common.h"
#include "bi/canvas.h"

int main(int argc, char* argv[])
{
  BiContext* context = make_context(__FILE__);
  // Sprite
  BiNode* sprite = make_sprite_with_anchor("assets/tester.png",0,0);
  BiNode* face = make_sprite("assets/face01.png");
  bi_node_set_opacity(face, 128);
  bi_node_add_node(sprite,face);
  bi_node_set_position(sprite,10,10);
  bi_node_set_position(face,sprite->w/2.0,sprite->h/2.0);
  bi_node_set_degree(face,45);
  // Canvas
  BiCanvas* canvas = bi_canvas_init(ALLOC(BiCanvas),256,256);
  canvas->shader = &context->default_shader;
  canvas->textures[0] = sprite->texture;
  canvas->textures[1] = face->texture;
  bi_canvas_clear(canvas,0,0,0,0);
  bi_canvas_draw(canvas,sprite);
  // Sprite from Canvas
  BiNode *canvas_sprite = bi_node_init(ALLOC(BiNode));
  bi_node_set_size(canvas_sprite, canvas->w,canvas->h);
  bi_node_set_position(canvas_sprite,192,0);
  BiTexture *canvas_texture = bi_canvas_to_texture(canvas,ALLOC(BiTexture));
  bi_node_set_texture(canvas_sprite,canvas_texture,0,0,canvas_texture->w,canvas_texture->h);
  // Layer
  BiLayer *layer = bi_layer_init(ALLOC(BiLayer));
  bi_add_layer(context,layer);
  BiNode* n = bi_layer_add_node(layer,make_bg("assets/check.png"));
  layer->textures[0] = n->texture;
  layer->textures[1] = canvas_texture;
  layer->textures[2] = sprite->texture;
  layer->textures[3] = face->texture;
  bi_node_add_node(n, canvas_sprite); // from Canvas
  bi_node_add_node(n, sprite); // Original
  // start
  bi_start_run_loop(context);
  return 0;
}
