#include "common.h"
#include <stdlib.h>

static BiNode* create_new_node(int x, int y)
{
  BiNode* node = bi_node_init(ALLOC(BiNode));
  bi_node_set_anchor(node,0.5,0.5);
  bi_node_set_position(node,x,y);
  bi_node_set_size(node,40,80);
  node->tint = RGBA( rand()%0xff, rand()%0xff, rand()%0xff, 0xff );
  bi_node_set_angle(node,rand()%360/180.0*M_PI);
  return node;
}

static bool on_click(BiContext* context,BiNode* n, int x, int y, int button, bool pressed)
{
  if(pressed) {
    if( bi_node_inside(n,x,y) ){
      int lx,ly;
      bi_node_transform_local(n,x,y,&lx,&ly);
      printf("(%d,%d) -> (%d,%d)\n",x,y,lx,ly);
      bi_node_set_angle(n,n->angle + 30 * M_PI / 180.0);
      return true; // swallow event
    }
  }
  return false; // through
}

int main(int argc,char* argv[])
{
  BiContext* context = make_context(__FILE__);
  // shader_node
  BiShaderNode *shader_node = bi_shader_node_init(ALLOC(BiShaderNode));
  BiNode* bg = bi_node_add_node(shader_node,make_bg("assets/check.png"));
  shader_node->textures[0] = bi_node_get_texture(bg);
  bi_node_add_node(&context->default_framebuffer_node,shader_node);
  // hit rect
  for(int x=0;x<5;x++){
    for(int y=0;y<3;y++){
      int xx = 120 + x*60;
      int yy = 100 + y*60;
      BiNode* node = create_new_node(xx,yy);
      bi_node_set_on_click(node, on_click);
      bi_node_add_node(shader_node,node);
    }
  }
  // start
  bi_start_run_loop(context);
  return 0;
}
