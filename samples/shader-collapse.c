#include "common.h"

#define FRAG "assets/shaders/collapse.frag"
#define VERT "assets/shaders/collapse.vert"

static void collapse(BiTimer* timer,double dt)
{
  static double t = 0;
  t += dt / 1000.0;
  BiNode *node = timer->userdata;
  node->shader_extra_data[0] = fabs(sin(t)); // in [0.0-1.0]
}

static void collapse_slow(BiTimer* timer,double dt)
{
  static double t = 0;
  t += dt / 1000.0 / 1.5;
  BiNode *node = timer->userdata;
  node->shader_extra_data[0] = fabs(sin(t)); // in [0.0-1.0]
}

int main(int argc, char* argv[])
{
  BiContext* context = make_context(__FILE__);
  // Background Layer
  BiLayer *bg_layer = bi_layer_init(ALLOC(BiLayer));
  BiNode* bg = bi_layer_add_node(bg_layer,make_bg("assets/check.png"));
  bg_layer->textures[0] = bg->texture;
  // Foreground Layer
  BiLayer *fg_layer = bi_layer_init(ALLOC(BiLayer));
  BiNode* face = bi_layer_add_node( fg_layer, make_sprite("assets/face01.png") );
  BiNode* mushroom = bi_layer_add_node( fg_layer, make_sprite("assets/mushroom.png") );
  bi_node_set_position(face,context->w/4,context->h/2);
  bi_node_set_position(mushroom,context->w/4*3,context->h/2);
  bi_node_set_scale(face,1.5,1.5);
  bi_node_set_scale(mushroom,3.0,3.0);
  fg_layer->textures[0] = face->texture;
  fg_layer->textures[1] = mushroom->texture;
  // timer for shader
  bi_node_add_timer(face, bi_timer_init(ALLOC(BiTimer),collapse_slow,0,-1,face));
  // timer for shader
  bi_node_add_timer(face, bi_timer_init(ALLOC(BiTimer),collapse,0,-1,mushroom));
  // shader
  fg_layer->shader = create_shader(VERT,FRAG);
  // add layer groups
  bi_layer_group_add_layer(&context->layers,bg_layer);
  bi_layer_group_add_layer(&context->layers,fg_layer);
  //
  bi_start_run_loop(context);
  return 0;
}
