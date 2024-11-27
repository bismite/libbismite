#include "common.h"
#include <math.h>

static void collapse(BiContext* ctx,BiTimer* timer,double dt)
{
  static double t = 0;
  t += dt / 2000.0;
  BiNode *node = timer->userdata;
  node->shader_extra_data[0] = fabs(sin(t)); // in [0.0-1.0]
}

int main(int argc, char* argv[])
{
  BiContext* context = make_context(__FILE__);
  // Background Layer
  BiShaderNode *bg_shader_node = bi_shader_node_init(ALLOC(BiShaderNode));
  BiNode* bg = bi_node_add_node(bg_shader_node,make_bg("assets/check.png"));
  bg_shader_node->textures[0] = bi_node_get_texture(bg);
  // Foreground Layer
  BiShaderNode *fg_shader_node = bi_shader_node_init(ALLOC(BiShaderNode));
  BiNode* face = bi_node_add_node( fg_shader_node, make_sprite("assets/face01.png") );
  BiNode* mushroom = bi_node_add_node( fg_shader_node, make_sprite("assets/mushroom.png") );
  bi_node_set_position(face,context->w/4,context->h/2);
  bi_node_set_position(mushroom,context->w/4*3,context->h/2);
  bi_node_set_scale(face,1.5,1.5);
  bi_node_set_scale(mushroom,3.0,3.0);
  fg_shader_node->textures[0] = bi_node_get_texture(face);
  fg_shader_node->textures[1] = bi_node_get_texture(mushroom);
  // timer for shader
  bi_node_add_timer(face, bi_timer_init(ALLOC(BiTimer),collapse,0,-1,face));
  // timer for shader
  bi_node_add_timer(face, bi_timer_init(ALLOC(BiTimer),collapse,0,-1,mushroom));
  // shader
  fg_shader_node->shader = create_shader_with_default_vertex_shader("assets/shaders/collapse.frag");
  // add shader_node groups
  bi_node_add_node(&context->default_framebuffer_node,bg_shader_node);
  bi_node_add_node(&context->default_framebuffer_node,fg_shader_node);
  //
  bi_start_run_loop(context);
  return 0;
}
