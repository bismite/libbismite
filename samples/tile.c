#include "common.h"

#define TILE_SIZE 32

static BiNode* create_tile(int x, int y,BiTexture *tex)
{
  int tx = rand()%(tex->w/TILE_SIZE) * TILE_SIZE;
  int ty = rand()%(tex->h/TILE_SIZE) * TILE_SIZE;
  BiNode* node = bi_node_init(ALLOC(BiNode));
  bi_node_set_texture(node,tex,tx,ty,TILE_SIZE,TILE_SIZE);
  bi_node_set_position(node,x,y);
  bi_node_set_size(node,TILE_SIZE,TILE_SIZE);
  return node;
}

int main(int argc,char* argv[])
{
  BiContext* context = bi_init_context(ALLOC(BiContext), 480, 320, 0, false, __FILE__);
  print_info(context);

  // texture
  BiTexture *texture = bi_texture_init_with_filename(ALLOC(BiTexture),"assets/tester.png",false);

  // layer
  BiLayer *layer = bi_layer_init(ALLOC(BiLayer));
  bi_add_layer(context,layer);
  layer->root = make_sprite_with_anchor("assets/map.png",0,0);
  layer->textures[0] = layer->root->texture;
  layer->textures[1] = texture;

  const int W = context->w/TILE_SIZE+1;
  const int H = context->h/TILE_SIZE+1;
  for(int i=0; i<W*H; i++){
    int x = i % W * TILE_SIZE;
    int y = i / W * TILE_SIZE;
    bi_node_add_node(layer->root,create_tile(x,y,texture));
  }

  // fps layer
  add_fps_layer(context,load_font());

  bi_start_run_loop(context);
  return 0;
}
