#include "common.h"

int main(int argc, char* argv[])
{
  BiContext* context = make_context(__FILE__);

  // Layer
  BiLayer *layer = bi_layer_init(ALLOC(BiLayer));
  bi_add_layer(context,layer);
  BiNode* root = bi_layer_add_node(layer,make_bg("assets/check.png"));

  // Texture
  BiTexture *texture = MAKE_TEXTURE("assets/tester.png");
  bi_texture_set_anti_alias(texture,true); // AA enabled

  // Sprite
  BiNode* sprite = bi_node_init(ALLOC(BiNode));
  bi_node_set_texture(sprite,texture,32,32,64,64);
  bi_node_set_size(sprite,64,64);
  bi_node_set_anchor(sprite,0.5,0.5);
  bi_node_set_position(sprite,context->w/2,context->h/2);
  bi_node_set_scale_x(sprite,4.0);
  bi_node_set_scale_y(sprite,4.0);

  // layer
  bi_node_add_node(root,sprite);
  layer->textures[0] = root->texture;
  layer->textures[1] = texture;

  bi_start_run_loop(context);
  return 0;
}
