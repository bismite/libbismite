#include "../samples/common.h"

bool ab_flag=true;
BiNode* rect_a=NULL;
BiNode* rect_b=NULL;

static bool invalid_callback(BiContext* context,BiNode* node, uint16_t scancode, uint32_t keycode, uint16_t mod, bool pressed)
{
  if(pressed) return false;
  printf("oops... %p\n",node);
  return false; // no swallow
}

static bool on_keyinput(BiContext* context,BiNode* node, uint16_t scancode, uint32_t keycode, uint16_t mod, bool pressed)
{
  if(pressed) return false;
  printf("key input %p\n",node);
  return false; // no swallow
}

static BiNode* new_node(int x,int y,BiColor c)
{
  BiNode *n = bi_node_init(ALLOC(BiNode));
  bi_node_set_on_keyinput(n, on_keyinput);
  bi_node_set_size(n,80,80);
  bi_node_set_position(n,x,y);
  n->color = c;
  return n;
}

static void node_add_and_remove(BiContext* ctx,BiTimer* timer,double dt)
{
  ab_flag = ! ab_flag;
  BiShaderNode *node = timer->userdata;
  if(ab_flag){
    rect_a = new_node(100,100,RGBA(0xff,0,0,0xff));
    bi_node_add_node(node,rect_a);
    // remove B
    if(rect_b){
      bi_node_set_on_keyinput(rect_b, invalid_callback);
      bi_node_remove_node(node,rect_b);
      bi_node_base_deinit((BiNodeBase*)rect_b);
      free(rect_b);
      rect_b=NULL;
    }
  }else{
    rect_b = new_node(300,100,RGBA(0,0xff,0,0xff));
    bi_node_add_node(node,rect_b);
    // remove A
    if(rect_a) {
      bi_node_set_on_keyinput(rect_a, invalid_callback);
      bi_node_remove_node(node,rect_a);
      bi_node_base_deinit((BiNodeBase*)rect_a);
      free(rect_a);
      rect_a=NULL;
    }
  }
}

int main(int argc,char* argv[])
{
  BiContext* context = make_context(__FILE__);
  BiShaderNode *shader_node = bi_shader_node_init(ALLOC(BiShaderNode));
  bi_node_add_timer(shader_node,bi_timer_init(ALLOC(BiTimer),node_add_and_remove,0,-1,shader_node));
  bi_node_add_node(&context->default_framebuffer_node,shader_node);
  bi_start_run_loop(context);
  return 0;
}
