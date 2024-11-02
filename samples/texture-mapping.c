#include "common.h"

int main(int argc, char* argv[])
{
  int left, bottom, right, top;
  int left2, bottom2, right2, top2;

  BiContext* context = make_context(__FILE__);

  // Texture
  BiTexture* tex = MAKE_TEXTURE("assets/tester.png");
  BiNode* background = make_bg("assets/check.png");

  // Layer
  BiLayer *layer = bi_layer_init(ALLOC(BiLayer));
  bi_layer_add_node(layer,background);
  layer->textures[0] = background->texture;
  layer->textures[1] = tex;
  bi_add_layer(context,layer);

  // No Cropping, Green Circle 32x32 in 32x32
  BiNode* n0 = bi_node_init(malloc(sizeof(BiNode)));
  bi_node_set_size(n0, 32,32);
  bi_node_set_position(n0,100,200);
  // image is upside-down
  left = 64; bottom = 96; right = 96; top = 64;
  left2 = 64; bottom2 = 96; right2 = 96; top2 = 64;
  // Normalize 0.0 to 1.0
  bi_node_set_texture_mapping(n0, tex,
    left/128.0, bottom/128.0, right/128.0, top/128.0,
    left2/128.0, bottom2/128.0, right2/128.0, top2/128.0
  );
  bi_layer_add_node(layer,n0);

  // With Cropping, Green Circle 32x32 in 64x64
  BiNode* n1 = bi_node_init(malloc(sizeof(BiNode)));
  bi_node_set_size(n1, 64,64);
  bi_node_set_position(n1,200,120);
  // image is upside-down
  left = 48; bottom = 112; right = 112; top = 48;
  left2 = 64; bottom2 = 96; right2 = 96; top2 = 64;
  // Normalize 0.0 to 1.0
  bi_node_set_texture_mapping(n1, tex,
    left/128.0, bottom/128.0, right/128.0, top/128.0,
    left2/128.0, bottom2/128.0, right2/128.0, top2/128.0
  );
  bi_layer_add_node(layer,n1);

  // Mapping (via Cropping)
  //   Red Circle 32x32 in 64x64
  BiNode* n2 = bi_node_init(malloc(sizeof(BiNode)));
  bi_node_set_size(n2, 64,64);
  bi_node_set_position(n2,300,80);
  bi_node_set_cropped_texture(n2, tex,
    32,96,32,32, 16,16, 64,64 );
  bi_layer_add_node(layer,n2);

  //
  bi_start_run_loop(context);
  return 0;
}
