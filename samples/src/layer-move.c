#include "common.h"
#include <stdlib.h>

static BiNode* create_new_node(int x, int y,BiTexture *tex)
{
    BiNode* node = malloc(sizeof(BiNode));
    bi_node_init(node);

    // texture
    node->texture_mapping = malloc(sizeof(BiTextureMapping));
    bi_texture_mapping_init(node->texture_mapping,tex);
    int tw = 32;
    int th = 32;
    int tx = rand()%(tex->w/tw) * tw;
    int ty = rand()%(tex->h/th) * th;
    bi_texture_mapping_set_bound(node->texture_mapping,tx,ty,tw,th);
    bi_node_set_position(node,x,y);
    bi_node_set_size(node,tw,th);
    bi_set_color( node->color, 0xFF, 0xFF, 0xFF, 0xFF);
    return node;
}

static bool on_move_cursor(BiContext *context,BiNode* n, int x, int y)
{
    BiLayer* l = n->userdata;
    int layer_w = 32*64 * 0.5;
    int layer_h = 32*64 * 0.5;
    l->camera_x = (float)x / context->w * layer_w;
    l->camera_y = (float)y / context->h * layer_h;
    return true;
}

#ifdef __EMSCRIPTEN__
static bool on_move_finger(BiContext* context, BiNode* n, float x, float y, int64_t finger_id)
{
    return on_move_cursor(context, n, x, y);
}
#endif


int main(int argc,char* argv[])
{
  BiContext* context = malloc(sizeof(BiContext));
  bi_init_context(context, 480, 320, 0, false, __FILE__);
  print_info(context);

  bi_set_color(context->color,32,32,0,0xff);

  //
  // tile layer
  //
  BiLayer *layer = malloc(sizeof(BiLayer));
  bi_layer_init(layer);
  layer->projection_centering = true;
  bi_layer_set_z_order(layer,-1);

  bi_add_layer(context,layer);

  // texture
  BiTexture *tex = malloc(sizeof(BiTexture));
  bi_texture_init_with_filename(tex,"assets/tile.png",false);
  layer->textures[0] = tex;

  // root node
  BiNode* tiles = malloc(sizeof(BiNode));
  bi_node_init(tiles);
  layer->root = tiles;
  tiles->scale_x = tiles->scale_y = 0.5;
  tiles->userdata = layer;

  bi_node_set_on_move_cursor(tiles, on_move_cursor);
#ifdef __EMSCRIPTEN__
  bi_node_set_on_move_finger(tiles, on_move_finger);
#endif

  for(int x=0; x<64; x++) {
    for(int y=0; y<64; y++){
      BiNode* node = create_new_node(x*32,y*32,tex);
      bi_node_add_node(tiles,node);
    }
  }

  //
  // fps layer
  //
  BiFontAtlas *font = load_font();
  add_fps_layer(context,font);

  bi_start_run_loop(context);
  return 0;
}
