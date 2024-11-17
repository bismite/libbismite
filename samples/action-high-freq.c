#include "common.h"
#include <bi/ext/action.h>

int main(int argc, char* argv[])
{
  BiContext* context = make_context(__FILE__);

  // shader_node
  BiShaderNode *shader_node = bi_shader_node_init(ALLOC(BiShaderNode));
  BiNode *root = bi_node_add_node(shader_node,make_bg("assets/check.png"));
  bi_node_add_node(&context->default_framebuffer_node,shader_node);

  // texture
  BiTexture* t = MAKE_TEXTURE("assets/face01.png");
  shader_node->textures[0] = bi_node_get_texture(root);
  shader_node->textures[1] = t;

  // 2degree/4msec x 540 = 1080degree/2160msec
  const int degree = 2;
  const int duration = 4;
  const int repeat = 540;
  // Blue
  BiNode* face_blue = make_sprite_from_texture(t);
  face_blue->tint = RGBA(0,0,0xFF,0xFF);
  bi_node_set_position(face_blue,240,320/3*2);
  bi_node_add_node(root,face_blue);
  {
    BiAction* actions[repeat];
    for(int i=0;i<repeat;i++){
      actions[i] = (BiAction*)bi_action_rotate_by_init( ALLOC(BiActionRotate),duration,degree);
    }
    BiAction* seq = (BiAction*)bi_action_sequence_init(ALLOC(BiActionSequence),repeat,actions);
    bi_add_action(face_blue,seq);
    bi_action_start(seq);
  }
  // Red
  BiNode* face_red = make_sprite_from_texture(t);
  face_red->tint = RGBA(0xFF,0,0,0xFF);
  bi_node_set_position(face_red,240,320/3*1);
  bi_node_add_node(root,face_red);
  {
    BiAction* rot = (BiAction*)bi_action_rotate_by_init(ALLOC(BiActionRotate),duration,degree);
    bi_action_set_repeat(rot,repeat);
    bi_add_action(face_red,rot);
    bi_action_start(rot);
  }

  bi_start_run_loop(context);
  return 0;
}
