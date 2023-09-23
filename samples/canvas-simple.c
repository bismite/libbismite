#include "common.h"
#include "bi/canvas.h"

int main(int argc, char* argv[])
{
  BiContext* context = make_context(__FILE__);

  // Sprite
  BiNode* face = make_sprite("assets/face01.png");
  bi_node_set_anchor(face,0,0);
  bi_node_set_position(face,0,128);
  bi_node_set_degree(face,0);

  // draw to Canvas
  BiCanvas* canvas = bi_canvas_init(ALLOC(BiCanvas),256,256);
  canvas->shader = &context->default_shader;
  canvas->textures[0] = face->texture;
  bi_canvas_clear(canvas,0,0xff,0,0xff);
  bi_canvas_draw(canvas,face);

  // Canvas Texture
  BiTexture *canvas_texture = bi_canvas_to_texture(canvas,ALLOC(BiTexture));

  // Sprite from Canvas (all)
  BiNode *canvas_sprite = bi_node_init(ALLOC(BiNode));
  bi_node_set_size(canvas_sprite, canvas->w,canvas->h);
  bi_node_set_position(canvas_sprite,32,32);
  bi_node_set_texture(canvas_sprite,canvas_texture,0,0,canvas_texture->w,canvas_texture->h);

  // Sprite from Canvas (partial)
  BiNode *canvas_sprite2 = bi_node_init(ALLOC(BiNode));
  bi_node_set_size(canvas_sprite2, 128,128);
  bi_node_set_position(canvas_sprite2,32+canvas->w+32,32);
  bi_node_set_texture(canvas_sprite2,canvas_texture,0,0,128,128);

  // layer
  BiLayer *layer = bi_layer_init(ALLOC(BiLayer));
  BiNode* root = bi_layer_add_node(layer,make_bg("assets/check.png") );
  bi_node_add_node(root, canvas_sprite);
  bi_node_add_node(root, canvas_sprite2);
  layer->textures[0] = root->texture;
  layer->textures[1] = canvas_texture;
  bi_add_layer(context,layer);

  bi_start_run_loop(context);
  return 0;
}
