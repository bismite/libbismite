#include "common.h"

static void rotate(BiContext* ctx,BiTimer* timer,double dt)
{
  BiNode *node = timer->userdata;
  bi_node_set_angle(node, node->angle + dt*0.01);
}

static void pause_rotate(BiContext* ctx,BiTimer* timer,double dt)
{
  BiTimer* rotate_timer = timer->userdata;
  BiNode* node = rotate_timer->userdata;
  bi_timer_pause(rotate_timer);
  if(timer->count==0){
    bi_node_remove_timer(node,timer);
    free(timer);
  }
}
static void resume_rotate(BiContext* ctx,BiTimer* timer,double dt)
{
  BiTimer* rotate_timer = timer->userdata;
  BiNode* node = rotate_timer->userdata;
  bi_timer_resume(rotate_timer);
  if(timer->count==0){
    bi_node_remove_timer(node,timer);
    free(timer);
  }
}

int main(int argc,char* argv[])
{
  BiContext* context = make_context(__FILE__);
  // shader_node
  BiShaderNode *shader_node = bi_shader_node_init(ALLOC(BiShaderNode));
  bi_add_shader_node(context,shader_node);
  BiNode* bg = bi_shader_node_add_node(shader_node,make_bg("assets/check.png"));
  BiNode* face = bi_shader_node_add_node(shader_node, make_sprite("assets/face01.png"));
  bi_node_set_position(face,context->w/2,context->h/2);
  shader_node->textures[0] = bg->texture;
  shader_node->textures[1] = face->texture;
  // rotate
  BiTimer *rotate_timer = bi_timer_init(ALLOC(BiTimer),rotate,10,-1,face);
  bi_node_add_timer(face,rotate_timer);
  // pause rotate
  bi_node_add_timer(face,bi_timer_init(ALLOC(BiTimer),pause_rotate,1000,2,rotate_timer));
  // resume rotate
  bi_node_add_timer(face,bi_timer_init(ALLOC(BiTimer),resume_rotate,1500,2,rotate_timer));
  // start
  bi_start_run_loop(context);
  return 0;
}
