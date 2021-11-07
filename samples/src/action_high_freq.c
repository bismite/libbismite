#include "common.h"
#include <bi/ext/action.h>

static BiAction* alloc_action(size_t size, void* payload)
{
  BiAction* action = malloc(sizeof(BiAction));
  bi_action_init(action);
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
  BiContext* context = malloc(sizeof(BiContext));
  bi_init_context(context, 480, 320, 60, false, __FILE__);
  print_info(context);

  // layer
  BiLayer *layer = malloc(sizeof(BiLayer));
  bi_layer_init(layer);

  // root node
  BiNode* root = malloc(sizeof(BiNode));
  bi_node_init(root);
  layer->root = root;

  BiNode* bg = make_sprite("assets/check.png");
  bi_node_set_position(bg,context->w/2,context->h/2);
  bi_node_add_node(root,bg);

  // texture node
  BiTextureMapping* tm = make_texture_mapping("assets/face01.png");
  BiNode* face = make_sprite_from_mapping(tm);
  bi_node_set_position(face,240,320/3*1);
  bi_node_add_node(root,face);
  BiNode* face2 = make_sprite_from_mapping(tm);
  bi_node_set_position(face2,240,320/3*2);
  bi_node_add_node(root,face2);

  layer->textures[0] = bg->texture_mapping->texture;
  layer->textures[1] = tm->texture;

  // action
  add_action(face);
  add_action2(face2);

  bi_add_layer(context,layer);

  bi_start_run_loop(context);
  return 0;
}
