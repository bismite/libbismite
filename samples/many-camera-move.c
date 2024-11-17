#include "common.h"
#include <math.h>

#define TILE_SIZE 64
#define MAP_SIZE 64
#define SCALE 0.25

static void shader_camera_position(BiShaderNode*l, float angle)
{
  float r = TILE_SIZE*MAP_SIZE*SCALE * 0.5;
  l->camera_x = r - 480/2 + r*cos(angle);
  l->camera_y = r - 320/2 + r*sin(angle);
}

static void lookaround(BiContext* ctx,BiTimer* timer,double dt)
{
  BiShaderNode* shader_node = timer->userdata;
  static float angle = 0;
  angle += dt*0.0005;
  shader_camera_position(shader_node,angle);
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
  BiContext* context = make_context(__FILE__);

  // texture
  BiTexture *tex = bi_texture_init_with_filename(ALLOC(BiTexture),"assets/tester.png",false);
  // shader_node
  BiShaderNode *shader_node = bi_shader_node_init(ALLOC(BiShaderNode));
  bi_node_add_node(&context->default_framebuffer_node,shader_node);
  BiNode* root = bi_node_add_node(shader_node, bi_node_init(ALLOC(BiNode)));
  bi_node_set_scale(root,SCALE,SCALE);
  shader_node->textures[0] = tex;
  // tiling
  for(int x=0; x<MAP_SIZE; x++) {
    for(int y=0; y<MAP_SIZE; y++){
      bi_node_add_node(root,create_tile(x*TILE_SIZE,y*TILE_SIZE,tex));
    }
  }
  // look around
  shader_camera_position(shader_node,0);
  bi_shader_node_add_timer(shader_node,bi_timer_init(ALLOC(BiTimer),lookaround,0,-1,shader_node));
  // start
  bi_start_run_loop(context);
  return 0;
}
