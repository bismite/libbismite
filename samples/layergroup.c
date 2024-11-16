#include "common.h"

BiFramebufferNode* canvas = NULL;

static void save(BiContext* ctx,BiTimer* t,double dt)
{
  // bi_framebuffer_save_png_image(&canvas->framebuffer,"0_layergroup.png");
}

// - Context LayerGroup
//   - Layer A
//     - Sprite(Check)
//     - Sprite(tester)
//     - Rect A(Red)
//   - LayerGroup
//     - Layer B
//       - Sprite(Face)
//       - Rect B(Green)
int main(int argc, char* argv[])
{
  BiContext* context = make_context(__FILE__);
  context->color = RGBA32(0x330000FF);

  // Layer A
  BiLayer *layer_a = bi_layer_init(ALLOC(BiLayer));
  bi_add_layer(context,layer_a);
  BiNode *bg = bi_layer_add_node(layer_a,make_bg("assets/check.png"));
  layer_a->textures[0] = bg->texture;
  // Sprites for Layer A
  BiNode* tester = make_sprite("assets/tester.png");
  bi_node_set_position(tester,480/4,320*3/4);
  bi_node_add_node(bg,tester);
  layer_a->textures[1] = tester->texture;
  // Rectangle A
  BiNode* rect_a = bi_node_init(ALLOC(BiNode));
  rect_a->color = RGBA(0xff,0,0,128);
  bi_node_set_size(rect_a,100,100);
  bi_node_set_position(rect_a,480/4,50);
  bi_node_add_node(bg,rect_a);

  onupdate(bg,save);

  // Layer B
  BiLayer* layer_b = bi_layer_init(ALLOC(BiLayer));
  // Sprite
  BiNode* face = bi_layer_add_node(layer_b,make_sprite("assets/face01.png"));
  layer_b->textures[0] = face->texture;
  bi_node_set_position(face,480/4*3,320*3/4);
  bi_layer_add_node(layer_b,face);
  // Rectangle B
  BiNode* rect_b = bi_node_init(ALLOC(BiNode));
  rect_b->color = RGBA(0,0xff,0,128);
  bi_node_set_size(rect_b,100,100);
  bi_node_set_position(rect_b,480/4*3,50);
  bi_layer_add_node(layer_b,rect_b);
  // Layer Group
  canvas = bi_framebuffer_node_init(ALLOC(BiFramebufferNode));
  bi_framebuffer_node_add_layer(canvas,layer_b);
  bi_framebuffer_node_add_framebuffer_node(&context->layers,canvas);
  //
  bi_start_run_loop(context);
  return 0;
}
