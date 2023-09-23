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
  BiContext* context = make_context(__FILE__);
  BiTexture *texture = bi_texture_init_with_filename(ALLOC(BiTexture),"assets/tester.png",false);
  BiLayer *layer = bi_layer_init(ALLOC(BiLayer));
  bi_add_layer(context,layer);
  layer->textures[0] = texture;
  // tiling
  const int COL = context->w/TILE_SIZE+1;
  const int ROW = context->h/TILE_SIZE+1;
  for(int i=0; i<COL*ROW; i++){
    int x = i % COL * TILE_SIZE;
    int y = i / COL * TILE_SIZE;
    bi_layer_add_node(layer,create_tile(x,y,texture));
  }
  // start
  bi_start_run_loop(context);
  return 0;
}
