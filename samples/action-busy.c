#include "common.h"
#include <bi/ext/action.h>

static void add_action(BiNode *node)
{
  // rotate action
  BiActionRotate* rot = bi_action_rotate_by_init(ALLOC(BiActionRotate),5000,180);
  rot->action.on_finish_callback_context = "rot";
  bi_action_set_repeat((BiAction*)rot,-1);
  bi_add_action(node,(BiAction*)rot);
  bi_action_start((BiAction*)rot);

  // move ->
  BiActionMove* move1 = bi_action_move_by_init(ALLOC(BiActionMove),5000,480,0);

  // move <-
  BiActionMove* move2 = bi_action_move_by_init(ALLOC(BiActionMove),5000,-480,0);

  // sequence
  BiAction* actions[] = { (BiAction*)move1, (BiAction*)move2 };
  BiActionSequence* seq = bi_action_sequence_init( ALLOC(BiActionSequence),2,actions);
  bi_action_set_repeat((BiAction*)seq,-1);
  // add action to node
  bi_add_action(node,(BiAction*)seq);
  bi_action_start((BiAction*)seq);
}

static void add_action2(BiNode *node)
{
  BiAction* tmp[10];

  // sequence move ->
  for(int i=0;i<10;i++){
    BiAction* a = (BiAction*)bi_action_move_by_init(ALLOC(BiActionMove),500,48,0);
    tmp[i] = a;
  }
  BiAction *seq_right = (BiAction*)bi_action_sequence_init(ALLOC(BiActionSequence),10,tmp);

  // sequence move <-
  for(int i=0;i<10;i++){
    BiAction* a = (BiAction*)bi_action_move_by_init(ALLOC(BiActionMove),500,-48,0);
    tmp[i] = a;
  }
  BiAction *seq_left = (BiAction*)bi_action_sequence_init(ALLOC(BiActionSequence),10,tmp);

  // repeat sequence
  tmp[0] = seq_right;
  tmp[1] = seq_left;
  BiActionSequence* seq_right_left = bi_action_sequence_init(ALLOC(BiActionSequence),2,tmp);
  bi_action_set_repeat((BiAction*)seq_right_left,-1);

  // add action to node
  bi_add_action(node,(BiAction*)seq_right_left);
  bi_action_start((BiAction*)seq_right_left);
}

static bool on_click(BiContext* context,BiNode* n, int x, int y, int button, bool pressed)
{
  if(pressed){
    for(int i=0;i<2;i++){
      printf("delaying...\n");
      SDL_Delay(1000);
    }
  }
  return true;
}

int main(int argc, char* argv[])
{
  BiContext* context = make_context(__FILE__);
  context->max_delta = 5000;

  // shader_node
  BiShaderNode *shader_node = bi_shader_node_init(ALLOC(BiShaderNode));
  BiNode* root = bi_shader_node_add_node(shader_node,make_bg("assets/check.png"));
  bi_add_shader_node(context,shader_node);

  // texture node
  BiTexture* tex = MAKE_TEXTURE("assets/face01.png");
  BiNode* face = make_sprite_from_texture(tex);
  bi_node_set_position(face,0,320/3*1);
  bi_node_add_node(root,face);
  BiNode* face2 = make_sprite_from_texture(tex);
  bi_node_set_position(face2,0,320/3*2);
  bi_node_add_node(root,face2);

  shader_node->textures[0] = root->texture;
  shader_node->textures[1] = tex;

  // action
  add_action(face);
  add_action2(face2);

  // callback
  bi_node_set_on_click(root, on_click);

  bi_start_run_loop(context);
  return 0;
}
