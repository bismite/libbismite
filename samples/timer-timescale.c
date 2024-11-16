#include "common.h"

static void rotate_face(BiContext* ctx,BiTimer* timer,double dt)
{
  BiNode *node = timer->userdata;
  bi_node_set_angle(node, node->angle + dt * M_PI/180);
}

int main(int argc,char* argv[])
{
  BiContext* context = make_context(__FILE__);
  // shader_node
  BiShaderNode *shader_node = bi_shader_node_init(ALLOC(BiShaderNode));
  BiNode* root = bi_shader_node_add_node(shader_node,make_bg("assets/check.png"));
  bi_add_shader_node(context,shader_node);
  // sprites
  float TIMESCALES[4] = {2.0, 1.0, 0.5, 0};
  BiTexture* tex = MAKE_TEXTURE("assets/face01.png");
  for(int i=0;i<4;i++){
    BiNode* face = make_sprite_from_texture(tex);
    bi_node_add_node(root,face);
    bi_node_set_position(face,i*480/5+480/5,160);
    bi_node_add_timer(face, bi_timer_init(ALLOC(BiTimer), rotate_face, 0, -1, face) );
    face->time_scale = TIMESCALES[i];
  }
  // textures
  shader_node->textures[0] = root->texture;
  shader_node->textures[1] = tex;
  // start
  bi_start_run_loop(context);
  return 0;
}
