#include "common.h"

static void rotate(BiContext* ctx,BiTimer* timer,double dt)
{
  BiNode *node = timer->userdata;
  bi_node_set_angle(node, node->angle + dt*0.01);
}

static void kill_rotate(BiContext* ctx,BiTimer* timer,double dt)
{
  BiTimer *timer_rotate = timer->userdata;
  BiNode *node = timer_rotate->userdata;
  bi_node_remove_timer(node,timer_rotate);
  free(timer_rotate);
  bi_node_remove_timer(node,timer);
  free(timer);
}

int main(int argc,char* argv[])
{
  BiContext* context = make_context(__FILE__);
  // shader_node
  BiShaderNode *shader_node = bi_shader_node_init(ALLOC(BiShaderNode));
  bi_add_shader_node(context,shader_node);
  BiNode* root = bi_shader_node_add_node(shader_node,make_bg("assets/check.png"));
  BiNode* face = make_sprite("assets/face01.png");
  bi_node_set_position(face,context->w/2,context->h/2);
  bi_node_add_node(root,face);
  shader_node->textures[0] = root->texture;
  shader_node->textures[1] = face->texture;
  // rotate and kill
  BiTimer *timer_rotate = bi_node_add_timer(face,bi_timer_init(ALLOC(BiTimer),rotate,10,-1,face));
  bi_node_add_timer(face,bi_timer_init(ALLOC(BiTimer),kill_rotate,1000,1,timer_rotate));
  // start
  bi_start_run_loop(context);
  return 0;
}
