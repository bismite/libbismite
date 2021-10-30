#include "common.h"
#include <bi/ext/action.h>

struct ActionsContainer {
  BiAction* actions[0xFF];
  size_t num;
};

static void callback_change_color(BiAction* action, void* payload)
{
  BiNode *node = action->node;
  bi_set_color(node->color, rand()%0xFF, rand()%0xFF, rand()%0xFF, 0xFF);
}

static BiAction* alloc_action(size_t size, void* func, void* payload)
{
  BiAction* action = malloc(sizeof(BiAction));
  bi_action_init(action);
  action->action_data = malloc(size);
  action->on_finish = func;
  action->on_finish_callback_context = payload;
  return action;
}

static void dealloc_action(BiAction *action)
{
  free(action->action_data);
  free(action);
}

static void add_action(BiNode *node)
{
  // move, call, move
  BiAction* call0 = alloc_action( 0, callback_change_color, node );
  bi_action_base_init(call0);
  BiAction* move1 = alloc_action( sizeof(BiActionMove), NULL, NULL );
  bi_action_move_to_init(move1,1000,0,0);
  BiAction* call1 = alloc_action( 0, callback_change_color, node );
  bi_action_base_init(call1);
  BiAction* move2 = alloc_action( sizeof(BiActionMove), NULL, NULL );
  bi_action_move_to_init(move2,1000,480,320);
  BiAction* call2 = alloc_action( 0, callback_change_color, node );
  bi_action_base_init(call2);
  // sequence
  BiAction* actions[] = { call0, move1, call1, move2, call2 };
  BiAction* seq = alloc_action( sizeof(BiActionSequence), NULL, NULL );
  bi_action_sequence_init(seq,5,actions);
  // repeat
  BiAction* repeat = alloc_action( sizeof(BiActionRepeat), NULL, NULL );
  bi_action_repeat_init(repeat,seq);
  // add action to node
  bi_add_action(node,repeat);
  bi_action_start(repeat);

  // add rotate action
  BiAction* rot = alloc_action( sizeof(BiActionRotate), NULL, NULL );
  bi_action_rotate_by_init(rot,2000,360);
  bi_add_action(node,rot);
  bi_action_start(rot);

  struct ActionsContainer *container = malloc(sizeof(struct ActionsContainer));
  container->actions[7] = rot;
  container->actions[6] = repeat;
  container->actions[5] = seq;
  container->actions[4] = call2;
  container->actions[3] = move2;
  container->actions[2] = call1;
  container->actions[1] = move1;
  container->actions[0] = call0;
  container->num = 8;
  node->userdata = container;
}

static void remove_action(BiNode *node)
{
  struct ActionsContainer *container = node->userdata;
  for(int i=0;i<container->num;i++){
    bi_remove_action(node,container->actions[i]);
    dealloc_action(container->actions[i]);
  }
  free(container->actions);
  node->userdata = NULL;
}

static bool on_click(BiContext* context,BiNode* n, int x, int y, int button, bool pressed)
{
  if(pressed==false) {
    if(n->userdata != NULL) {
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
  BiContext* context = malloc(sizeof(BiContext));
  bi_init_context(context, 480, 320, 0, false, __FILE__);
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
  BiNode* face = make_sprite("assets/face01.png");
  bi_node_set_position(face,240,160);
  bi_node_add_node(root,face);
  layer->textures[0] = bg->texture_mapping->texture;
  layer->textures[1] = face->texture_mapping->texture;

  // callback
  bi_node_set_on_click(face, on_click);

  // action
  add_action(face);

  bi_add_layer(context,layer);

  bi_start_run_loop(context);
  return 0;
}
