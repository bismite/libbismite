#include "common.h"
#include <bi/ext/action.h>

static void callback(BiAction* action,void* context)
{
  printf("Scale %f,%f\n",action->node->scale_x, action->node->scale_y);
}

int main(int argc, char* argv[])
{
  BiContext* context = make_context(__FILE__);

  // sprite
  BiTexture *texture = MAKE_TEXTURE("assets/face01.png");
  // shader_node
  BiShaderNode *shader_node = bi_shader_node_init(ALLOC(BiShaderNode));
  BiNode* root = bi_node_add_node(shader_node,make_bg("assets/check.png"));
  bi_node_add_node(&context->default_framebuffer_node,shader_node);
  shader_node->textures[0] = bi_node_get_texture(root);
  shader_node->textures[1] = texture;

  // face green
  BiNode* face_green = make_sprite_from_texture(texture);
  face_green->tint = RGBA(0,0xff,0,0xff);
  bi_node_set_position(face_green,240,160);
  bi_node_add_node(root,face_green);
  // face red
  BiNode* face_red = make_sprite_from_texture(texture);
  face_red->tint = RGBA(0xff,0,0,0xff);
  bi_node_set_position(face_red,120,80);
  bi_node_add_node(root,face_red);
  bi_node_set_scale(face_red,0.5,0.5); // x0.5
  // face blue
  BiNode* face_blue = make_sprite_from_texture(texture);
  face_blue->tint = RGBA(0,0,0xff,0xff);
  bi_node_set_position(face_blue,360,240);
  bi_node_add_node(root,face_blue);
  bi_node_set_scale(face_blue,0.5,0.5); // x0.5

  // Scale By
  BiActionScale* scale_by = bi_action_scale_by_init(ALLOC(BiActionScale),5000,2,2);
  scale_by->action.on_finish = callback;
  bi_add_action(face_red,(BiAction*)scale_by);
  bi_action_start((BiAction*)scale_by);

  // Scale To
  BiActionScale* scale_to = bi_action_scale_to_init(ALLOC(BiActionScale),5000,2,2);
  scale_to->action.on_finish = callback;
  bi_add_action(face_blue,(BiAction*)scale_to);
  bi_action_start((BiAction*)scale_to);

  bi_start_run_loop(context);
  return 0;
}
