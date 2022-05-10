#include "common.h"
#include <bi/ext/action.h>

static BiAction* alloc_action(size_t size, void* payload)
{
  BiAction* action = bi_action_init(malloc(sizeof(BiAction)));
  action->action_data = malloc(size);
  action->on_finish = NULL;
  action->on_finish_callback_context = payload;
  return action;
}

static void add_action2(BiNode *node)
{
  // rotate high frequency
  BiAction* actions[0xFF];
  for(int i=0;i<0xFF;i++){
    BiAction* a = alloc_action( sizeof(BiActionRotate), NULL );
    bi_action_rotate_by_init(a,4,2);
    actions[i] = a;
  }
  BiAction *seq = alloc_action( sizeof(BiActionSequence), NULL );
  bi_action_sequence_init(seq,0xFF,actions);

  // add action to node
  bi_add_action(node,seq);
  bi_action_start(seq);
}

static void add_action(BiNode *node)
{
  // rotate high frequency
  BiAction* rot = alloc_action( sizeof(BiActionRotate), NULL );
  bi_action_rotate_by_init(rot,4,2);
  // repeat
  BiAction* repeat = alloc_action( sizeof(BiActionRepeat), NULL );
  bi_action_repeat_init(repeat,rot);
  // add action to node
  bi_add_action(node,repeat);
  bi_action_start(repeat);
}

int main(int argc, char* argv[])
{
  BiContext* context = bi_init_context(malloc(sizeof(BiContext)),480,320,0,false,__FILE__);
  print_info(context);

  // layer
  BiLayer *layer = bi_layer_init(malloc(sizeof(BiLayer)));
  layer->root = make_sprite_with_anchor("assets/check.png",0,0);
  bi_add_layer(context,layer);

  // texture node
  BiTexture* t = MAKE_TEXTURE("assets/face01.png");
  BiNode* face = make_sprite_from_texture(t);
  bi_node_set_position(face,240,320/3*1);
  bi_node_add_node(layer->root,face);
  BiNode* face2 = make_sprite_from_texture(t);
  bi_node_set_position(face2,240,320/3*2);
  bi_node_add_node(layer->root,face2);

  layer->textures[0] = layer->root->texture;
  layer->textures[1] = t;

  // action
  add_action(face);
  add_action2(face2);

  bi_start_run_loop(context);
  return 0;
}
