#include "common.h"

static void rotate(BiTimer* timer,double dt)
{
  BiNode *node = timer->userdata;
  bi_node_set_angle(node, node->angle + dt*0.01);
}

static void pause_rotate(BiTimer* timer,double dt)
{
  BiTimer* rotate_timer = timer->userdata;
  BiNode* node = rotate_timer->userdata;
  bi_timer_pause(rotate_timer);
  if(timer->count==0){
    bi_node_remove_timer(node,timer);
    free(timer);
  }
}
static void resume_rotate(BiTimer* timer,double dt)
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
  // layer
  BiLayer *layer = bi_layer_init(ALLOC(BiLayer));
  bi_add_layer(context,layer);
  set_texture(&layer->root,"assets/check.png");
  BiNode* face = make_sprite("assets/face01.png");
  bi_node_set_position(face,context->w/2,context->h/2);
  bi_node_add_node(&layer->root,face);
  layer->textures[0] = layer->root.texture;
  layer->textures[1] = face->texture;
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
