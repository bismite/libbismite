#include "common.h"
#include <stdlib.h>

static BiNode* create_new_node(int x, int y)
{
  BiNode* node = malloc(sizeof(BiNode));
  bi_node_init(node);
  node->anchor_x = node->anchor_y = 0.5;
  bi_node_set_position(node,x,y);
  bi_node_set_size(node,40,80);
  bi_set_color( node->color, rand()%0xFF, rand()%0xFF, rand()%0xFF, 0xFF);
  bi_node_set_angle(node,rand()%360/180.0*M_PI);
  return node;
}

static bool on_click(BiContext* context,BiNode* n, int x, int y, int button, bool pressed)
{
  if(pressed) {
    int lx,ly;
    bi_node_transform_local(n,x,y,&lx,&ly);
    if( bi_node_inside(n,x,y) ){
      bi_node_set_angle(n,n->angle + 30 * M_PI / 180.0);
      return true; // swallow event
    }
  }
  return false; // through
}

int main(int argc,char* argv[])
{
  srand( bi_get_now() );
  BiContext* context = malloc(sizeof(BiContext));
  bi_init_context(context, 480, 320, 0, true, __FILE__);
  print_info(context);

  // root node
  BiNode* root = malloc(sizeof(BiNode));
  bi_node_init(root);

  // hit rect
  for(int x=0;x<5;x++){
    for(int y=0;y<3;y++){
      int xx = 120 + x*60;
      int yy = 100 + y*60;
      BiNode* node = create_new_node(xx,yy);
      bi_node_set_on_click(node, on_click);
      bi_node_add_node(root,node);
    }
  }

  // layer
  BiLayer *layer = malloc(sizeof(BiLayer));
  bi_layer_init(layer);
  bi_add_layer(context,layer);
  layer->root = root;

  // fps layer
  BiFontAtlas *font = load_font();
  add_fps_layer(context,font);

  bi_start_run_loop(context);
  return 0;
}
