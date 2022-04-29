#include "common.h"
#include <bi/ext/action.h>

static void end_action(BiAction* action,void* context)
{
  bi_set_color(action->node->color, rand()%128+100, rand()%128+100, rand()%128+100, 0xff );
  printf("end action %s (%p) %lld\n", context?(char*)context:"", action, bi_get_now());
}

static BiAction* alloc_action(size_t size, void* payload)
{
  BiAction* action = malloc(sizeof(BiAction));
  bi_action_init(action);
  action->action_data = malloc(size);
  action->on_finish = end_action;
  action->on_finish_callback_context = payload;
  return action;
}

static void add_action(BiNode *node)
{
  // rotate action
  BiAction* rot = alloc_action( sizeof(BiActionRotate), "rot" );
  bi_action_rotate_by_init(rot,500,90);
  // repeat
  BiAction* repeat = alloc_action( sizeof(BiActionRepeat), "repeat" );
  bi_action_repeat_init(repeat,rot);
  // add action to node
  bi_add_action(node,repeat);
  bi_action_start(repeat);

  // move ->
  BiAction* move1 = alloc_action( sizeof(BiActionMove), "move" );
  bi_action_move_by_init(move1,5000,480,0);
  // move <-
  BiAction* move2 = alloc_action( sizeof(BiActionMove), "move2" );
  bi_action_move_by_init(move2,5000,-480,0);
  // sequence
  BiAction* actions[] = { move1,move2 };
  BiAction* seq = alloc_action( sizeof(BiActionSequence), "seq" );
  bi_action_sequence_init(seq,2,actions);
  // repeat sequence
  BiAction* repeat2 = alloc_action( sizeof(BiActionRepeat), "repeat2" );
  bi_action_repeat_init(repeat2,seq);
  // add action to node
  bi_add_action(node,repeat2);
  bi_action_start(repeat2);
}

static void add_action2(BiNode *node)
{
  // sequence move ->
  BiAction* actions[10];
  for(int i=0;i<10;i++){
    BiAction* a = alloc_action( sizeof(BiActionMove), NULL );
    bi_action_move_by_init(a,500,48,0);
    actions[i] = a;
  }
  BiAction *seq_right = alloc_action( sizeof(BiActionSequence), NULL );
  bi_action_sequence_init(seq_right,10,actions);

  // sequence move <-
  for(int i=0;i<10;i++){
    BiAction* a = alloc_action( sizeof(BiActionMove), NULL );
    bi_action_move_by_init(a,500,-48,0);
    actions[i] = a;
  }
  BiAction *seq_left = alloc_action( sizeof(BiActionSequence), NULL );
  bi_action_sequence_init(seq_left,10,actions);

  // repeat sequence
  BiAction* seq_right_left = alloc_action( sizeof(BiActionSequence), NULL );
  actions[0] = seq_right;
  actions[1] = seq_left;
  bi_action_sequence_init(seq_right_left,2,actions);
  BiAction* r = alloc_action( sizeof(BiActionRepeat), NULL );
  bi_action_repeat_init(r,seq_right_left);
  // add action to node
  bi_add_action(node,r);
  bi_action_start(r);
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
  BiContext* context = malloc(sizeof(BiContext));
  bi_init_context(context, 480, 320, 60, false, __FILE__);
  context->max_delta = 5000;
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
  BiTexture* tex = MAKE_TEXTURE("assets/face01.png");
  BiNode* face = make_sprite_from_texture(tex);
  bi_node_set_position(face,0,320/3*1);
  bi_node_add_node(root,face);
  BiNode* face2 = make_sprite_from_texture(tex);
  bi_node_set_position(face2,0,320/3*2);
  bi_node_add_node(root,face2);

  layer->textures[0] = bg->_texture;
  layer->textures[1] = tex;

  // action
  add_action(face);
  add_action2(face2);

  // callback
  bi_node_set_on_click(face, on_click);

  bi_add_layer(context,layer);

  bi_start_run_loop(context);
  return 0;
}
