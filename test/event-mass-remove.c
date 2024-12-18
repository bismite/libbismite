#include "../samples/common.h"

#define STAR_NUM 1000
BiShaderNode* shader_node=NULL;
BiNode* middleman=NULL;

static bool on_move_cursor(BiContext* context,BiNode* n, int x, int y)
{
  if( bi_node_inside(n,x,y) ){
    n->color = RGBA(0xff,0xff,0xff,0x80);
  }
  return false;
}

static void add_star(void* target)
{
  BiNode *n = bi_node_init(ALLOC(BiNode));
  bi_node_set_on_move_cursor(n, on_move_cursor);
  bi_node_set_size(n,10,10);
  bi_node_set_position(n,rand()%480,rand()%320);
  n->color = RGBA(rand()%0xff,rand()%0xff,rand()%0xff,0x80);
  bi_node_add_node(target,n);
}

static void blink(BiContext* ctx,BiTimer* timer,double dt)
{
  if(middleman->parent){
    bi_node_remove_node(shader_node,middleman);
  }else{
    bi_node_add_node(shader_node,middleman);
  }
  // Size=2000 or 1000
  printf("interaction_queue.size=%llu\n",ctx->interaction_queue.size);
}

int main(int argc,char* argv[])
{
  BiContext* context = make_context(__FILE__);
  shader_node = bi_shader_node_init(ALLOC(BiShaderNode));
  middleman = bi_node_init(ALLOC(BiNode));
  // middleman->interaction_enabled=false;
  bi_node_add_timer(shader_node,bi_timer_init(ALLOC(BiTimer),blink,0,-1,shader_node));
  bi_node_add_node(&context->default_framebuffer_node,shader_node);
  bi_node_add_node(shader_node,middleman);
  for(int i=0;i<STAR_NUM;i++){ add_star(middleman); }
  for(int i=0;i<STAR_NUM;i++){ add_star(shader_node); }
  bi_start_run_loop(context);
  return 0;
}
