#include "common.h"

#define TILE_SIZE 64
#define MAP_SIZE 64

static BiNode* create_tile(int x, int y,BiTexture *tex)
{
  BiNode* node = bi_node_init(malloc(sizeof(BiNode)));
  node->texture_mapping = bi_texture_mapping_init(malloc(sizeof(BiTextureMapping)),tex);
  int tx = rand()%(tex->w/TILE_SIZE) * TILE_SIZE;
  int ty = rand()%(tex->h/TILE_SIZE) * TILE_SIZE;
  bi_texture_mapping_set_bound(node->texture_mapping,tx,ty,TILE_SIZE,TILE_SIZE);
  bi_node_set_position(node,x,y);
  bi_node_set_size(node,TILE_SIZE,TILE_SIZE);
  return node;
}

static bool on_move_cursor(BiContext *context,BiNode* n, int x, int y)
{
  BiLayer* l = n->userdata;
  int layer_w = TILE_SIZE*MAP_SIZE * 0.5;
  int layer_h = TILE_SIZE*MAP_SIZE * 0.5;
  l->camera_x = (float)x / context->w * layer_w;
  l->camera_y = (float)y / context->h * layer_h;
  return true;
}

static bool on_move_finger(BiContext* context, BiNode* n, float x, float y, int64_t finger_id)
{
  return on_move_cursor(context, n, x, y);
}

int main(int argc,char* argv[])
{
  BiContext* context = bi_init_context(malloc(sizeof(BiContext)), 480, 320, 0, false, __FILE__);
  bi_set_color(context->color,32,32,0,0xff);
  print_info(context);

  // texture
  BiTexture *tex = bi_texture_init_with_filename(malloc(sizeof(BiTexture)),"assets/tester.png",false);

  // layer
  BiLayer *layer = bi_layer_init(malloc(sizeof(BiLayer)));
  layer->projection_centering = true;
  bi_add_layer(context,layer);
  layer->root = bi_node_init(malloc(sizeof(BiNode)));
  bi_node_set_scale(layer->root,0.5,0.5);
  layer->root->userdata = layer;
  layer->textures[0] = tex;

  bi_node_set_on_move_finger(layer->root, on_move_finger);
  bi_node_set_on_move_cursor(layer->root, on_move_cursor);

  // tiling
  for(int x=0; x<MAP_SIZE; x++) {
    for(int y=0; y<MAP_SIZE; y++){
      bi_node_add_node(layer->root,create_tile(x*TILE_SIZE,y*TILE_SIZE,tex));
    }
  }

  // fps layer
  add_fps_layer(context,load_font());

  bi_start_run_loop(context);
  return 0;
}
