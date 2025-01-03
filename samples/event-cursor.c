#include "common.h"

static bool on_click(BiContext* context,BiNode* n, int x, int y, int button, bool pressed)
{
  if(pressed) {
    bi_node_set_angle(n,n->angle + 30*3.1415/180.0);
    bi_node_set_position(n,x,y);
  }
  return true;
}

static bool on_move_cursor(BiContext* context,BiNode* n, int x, int y)
{
  bi_node_set_position(n,x,y);
  return true;
}

#ifdef __EMSCRIPTEN__
static bool on_touch(BiContext* context,BiNode* n, float x, float y, int64_t finger_id, bool pressed)
{
  if(pressed) {
    bi_node_set_position(n,x*480,y*320);
    bi_node_set_angle(n,n->angle + 30*3.1415/180.0);
  }
  return true;
}

static bool on_move_finger(BiContext* context,BiNode* n, float x, float y, int64_t finger_id)
{
  bi_node_set_position(n,x*480, y * 320);
  return true;
}
#endif

int main(int argc,char* argv[])
{
  BiContext* context = make_context(__FILE__);
  BiShaderNode *shader_node = bi_shader_node_init(ALLOC(BiShaderNode));
  BiNode* face = bi_node_add_node(shader_node, make_sprite("assets/face01.png"));
  bi_node_add_node(&context->default_framebuffer_node,shader_node);
  shader_node->textures[0] = bi_node_get_texture(face);
  bi_node_set_position(face,context->w/2,context->h/2);
  // set callbacks
  bi_node_set_on_click(face, on_click);
  bi_node_set_on_move_cursor(face, on_move_cursor);
#ifdef __EMSCRIPTEN__
  bi_node_set_on_move_finger(face, on_move_finger);
  bi_node_set_on_touch(face, on_touch);
#endif
  // start
  bi_start_run_loop(context);
  return 0;
}
