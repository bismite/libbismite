#include "common.h"

#define FRAG "assets/shaders/collapse.frag"
#define VERT "assets/shaders/collapse.vert"

static void update_collapse_height(BiTimer* timer,double dt)
{
  static double t=0;
  BiNode *node = timer->userdata;
  t += dt*0.001;
  float h = fabs(sin(t)); // in [0.0-1.0]
  node->shader_extra_data[0] = h;
}


int main(int argc, char* argv[])
{
  BiContext* context = bi_init_context(malloc(sizeof(BiContext)), 480, 320, 0, true, __FILE__);
  print_info(context);

  // Background
  BiLayer *bg = bi_layer_init(ALLOC(BiLayer));
  bg->root = make_sprite("assets/check.png");
  bi_node_set_position(bg->root,context->w/2,context->h/2);
  bg->textures[0] = bg->root->texture;

  // Foreground Layer Group
  BiLayer *fg = bi_layer_init(ALLOC(BiLayer));
  fg->root = make_sprite("assets/face01.png");
  fg->root->shader_extra_data[0] = 0;
  bi_node_set_position(fg->root,context->w/2,context->h/2);
  bi_node_set_scale(fg->root,2.0,2.0);
  fg->textures[0] = fg->root->texture;

  bi_node_add_timer(fg->root, bi_timer_init(ALLOC(BiTimer),update_collapse_height,0,-1,fg->root));

  // shader
  fg->shader = create_shader(VERT,FRAG);

  //
  bi_layer_group_add_layer(&context->layers,bg);
  bi_layer_group_add_layer(&context->layers,fg);

  //
  bi_start_run_loop(context);
  return 0;
}
