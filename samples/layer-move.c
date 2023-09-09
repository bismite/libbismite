#include "common.h"

#define TILE_SIZE 64
#define MAP_SIZE 64
#define SCALE 0.25

static void layer_camera_position(BiLayer*l, float angle)
{
  float r = TILE_SIZE*MAP_SIZE*SCALE * 0.5;
  l->camera_x = r - 480/2 + r*cos(angle);
  l->camera_y = r - 320/2 + r*sin(angle);
}

static void lookaround(BiTimer* timer,double dt)
{
  BiLayer* layer = timer->userdata;
  static float angle = 0;
  angle += dt*0.0005;
  layer_camera_position(layer,angle);
}

static BiNode* create_tile(int x, int y,BiTexture *tex)
{
  BiNode* node = bi_node_init(ALLOC(BiNode));
  int tx = rand()%(tex->w/TILE_SIZE) * TILE_SIZE;
  int ty = rand()%(tex->h/TILE_SIZE) * TILE_SIZE;
  bi_node_set_texture(node,tex,tx,ty,TILE_SIZE,TILE_SIZE);
  bi_node_set_position(node,x,y);
  bi_node_set_size(node,TILE_SIZE,TILE_SIZE);
  return node;
}

int main(int argc,char* argv[])
{
  BiContext* context = bi_init_context(ALLOC(BiContext), 480, 320, 0, false, __FILE__);
  context->color = RGBA(32,32,0,0xff);
  print_info(context);

  // texture
  BiTexture *tex = bi_texture_init_with_filename(ALLOC(BiTexture),"assets/tester.png",false);

  // layer
  BiLayer *layer = bi_layer_init(ALLOC(BiLayer));
  bi_add_layer(context,layer);
  layer->root = bi_node_init(ALLOC(BiNode));
  bi_node_set_scale(layer->root,SCALE,SCALE);
  layer->root->userdata = layer;
  layer->textures[0] = tex;

  // tiling
  for(int x=0; x<MAP_SIZE; x++) {
    for(int y=0; y<MAP_SIZE; y++){
      bi_node_add_node(layer->root,create_tile(x*TILE_SIZE,y*TILE_SIZE,tex));
    }
  }

  // look around
  layer_camera_position(layer,0);
  bi_layer_add_timer(layer,bi_timer_init(ALLOC(BiTimer),lookaround,0,-1,layer));

  // fps layer
  add_fps_layer(context,load_font());

  bi_start_run_loop(context);
  return 0;
}
