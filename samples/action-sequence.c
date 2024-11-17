#include "common.h"
#include <bi/ext/action.h>

BiAction* move1 = NULL;
BiAction* move2 = NULL;
BiAction* seq = NULL;

static void callback(BiAction* action,void* context)
{
  action->node->tint = RGBA(rand()%128+100, rand()%128+100, rand()%128+100, 0xff);
}

static void add_action(BiNode *node)
{
  BiAction* actions[2];
  actions[0] = (BiAction*)bi_action_move_by_init(ALLOC(BiActionMove),500,-60,0);
  actions[0]->on_finish = callback;
  actions[1] = (BiAction*)bi_action_move_by_init(ALLOC(BiActionMove),500,60,0);
  actions[1]->on_finish = callback;
  seq = (BiAction*)bi_action_sequence_init(ALLOC(BiActionSequence),2,actions);
  bi_action_set_repeat(seq,-1);
  bi_add_action(node,seq);
  bi_action_start(seq);
}

static void remove_action(BiNode *node)
{
  bi_remove_action(node, seq );
  free(seq);
  free(move1);
  free(move2);
  seq=NULL;
  move1=NULL;
  move2=NULL;
}

static bool on_click(BiContext* context,BiNode* n, int x, int y, int button, bool pressed)
{
  if(pressed==false) {
    if(seq) {
      remove_action(n);
    }else{
      bi_node_set_position(n,240,160);
      add_action(n);
    }
  }
  return true;
}

int main(int argc, char* argv[])
{
  BiContext* context = make_context(__FILE__);

  // shader_node
  BiShaderNode *shader_node = bi_shader_node_init(ALLOC(BiShaderNode));
  BiNode* root = bi_node_add_node(shader_node,make_bg("assets/check.png"));
  bi_node_add_node(&context->default_framebuffer_node,shader_node);

  // sprite
  BiNode* face = make_sprite("assets/face01.png");
  bi_node_set_position(face,240,160);
  bi_node_add_node(root,face);
  shader_node->textures[0] = bi_node_get_texture(root);
  shader_node->textures[1] = bi_node_get_texture(face);

  // callback
  bi_node_set_on_click(face, on_click);

  // action
  add_action(face);

  bi_start_run_loop(context);
  return 0;
}
