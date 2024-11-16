#include "common.h"
#include <bi/ext/action.h>

BiFramebufferNode* framebuffer_node_a;
BiFramebufferNode* framebuffer_node_b;

static void rotate_on_timer(BiContext* ctx,BiTimer* timer,double dt)
{
  BiNode *node = timer->userdata;
  bi_node_set_angle(node, node->angle + 2.0*M_PI/180.0);
}

static bool on_click(BiContext* context,BiNode* n, int x, int y, int button, bool pressed)
{
  if(pressed){
    framebuffer_node_a->time_scale = framebuffer_node_a->time_scale == 0 ? 1.0 : 0;
  }
  return true; // swallow
}

static BiFramebufferNode* make_framebuffer_node(BiContext* context, BiFramebufferNode* lg, int offset_y)
{
  bi_framebuffer_node_init(lg);
  bi_framebuffer_node_add_framebuffer_node(&context->shader_nodes, lg);
  // shader_node
  BiTexture* tex = MAKE_TEXTURE("assets/face01.png");
  BiShaderNode *shader_node = bi_shader_node_init(ALLOC(BiShaderNode));
  BiNode* root = bi_shader_node_add_node(shader_node,bi_node_init(ALLOC(BiNode)));
  bi_framebuffer_node_add_shader_node(lg,shader_node);
  shader_node->textures[0] = tex;
  // sprite
  BiNode* faces[2];
  for(int i=0;i<2;i++){
    BiNode* face = make_sprite_from_texture(tex);
    bi_node_set_position(face,480/3*(1+i%2),offset_y);
    bi_node_add_node(root,face);
    faces[i] = face;
  }
  // rotate by action
  faces[0]->tint = RGBA(0xFF,0,0,0xFF);
  BiAction* rot = (BiAction*)bi_action_rotate_by_init(ALLOC(BiActionRotate),0,2);
  bi_action_set_repeat(rot,-1);
  bi_add_action(faces[0],rot);
  bi_action_start(rot);
  // rotate by timer
  faces[1]->tint = RGBA(0,0,0xFF,0xFF);
  bi_node_add_timer(faces[1], bi_timer_init(ALLOC(BiTimer), rotate_on_timer, 0, -1, faces[1]));
  return lg;
}

int main(int argc, char* argv[])
{
  BiContext* context = make_context(__FILE__);
  // shader_node
  BiShaderNode *shader_node = bi_shader_node_init(ALLOC(BiShaderNode));
  BiNode* root = bi_shader_node_add_node(shader_node,make_bg("assets/check.png"));
  bi_node_set_on_click(root, on_click);
  bi_framebuffer_node_add_shader_node(&context->shader_nodes,shader_node);
  shader_node->textures[0] = root->texture;
  framebuffer_node_a = make_framebuffer_node(context, ALLOC(BiFramebufferNode), 320/3*2);
  framebuffer_node_b = make_framebuffer_node(context, ALLOC(BiFramebufferNode), 320/3*1);
  // start
  bi_start_run_loop(context);
  return 0;
}
