#include "common.h"
#include <bi/ext/action.h>

BiLayerGroup* main_layer_group;

static BiAction* alloc_action(size_t size, void* payload)
{
  BiAction* action = malloc(sizeof(BiAction));
  bi_action_init(action);
  action->action_data = malloc(size);
  action->on_finish = NULL;
  action->on_finish_callback_context = payload;
  return action;
}

static void rotate_on_update(BiContext* context, BiNode *node, double dt)
{
  bi_node_set_angle(node,node->angle + 2.0*M_PI/180.0);
}

static void rotate_on_timer(BiContext* context,BiTimer* timer,double dt)
{
  BiNode *node = timer->userdata;
  bi_node_set_angle(node, node->angle + 2.0*M_PI/180.0);
}

static bool on_click(BiContext* context,BiNode* n, int x, int y, int button, bool pressed)
{
  if(pressed){
    main_layer_group->time_scale = main_layer_group->time_scale == 0 ? 1.0 : 0;
    context->timers.scale = context->timers.scale == 0 ? 1.0 : 0;
  }
  return true;
}

int main(int argc, char* argv[])
{
  BiContext* context = malloc(sizeof(BiContext));
  bi_init_context(context, 480, 320, 60, false, __FILE__);
  print_info(context);

  // Layer Group
  main_layer_group = malloc(sizeof(BiLayerGroup));
  bi_layer_group_init(main_layer_group);
  bi_layer_group_add_layer_group(&context->layers, main_layer_group);

  // root node
  BiNode* root = malloc(sizeof(BiNode));
  bi_node_init(root);

  BiNode* bg = make_sprite("assets/check.png");
  bi_node_set_position(bg,context->w/2,context->h/2);
  bi_node_add_node(root,bg);

  // texture node
  BiTextureMapping* tm = make_texture_mapping("assets/face01.png");

  BiNode* faces[4];
  for(int i=0;i<4;i++){
    BiNode* face = make_sprite_from_mapping(tm);
    bi_node_set_position(face,480/3*(1+i%2),320/3*(1+i/2));
    bi_node_add_node(root,face);
    faces[i] = face;
  }

  // layer
  BiLayer *layer = malloc(sizeof(BiLayer));
  bi_layer_init(layer);
  layer->root = root;
  bi_layer_group_add_layer(main_layer_group,layer);
  layer->textures[0] = bg->texture_mapping->texture;
  layer->textures[1] = tm->texture;

  // pause
  bi_node_set_on_click(root, on_click);

  // rotate by action
  bi_set_color(faces[0]->color,0xFF,0,0,0xFF);
  BiAction* rot = alloc_action( sizeof(BiActionRotate), NULL );
  bi_action_rotate_by_init(rot,0,2);
  BiAction* repeat = alloc_action( sizeof(BiActionRepeat), NULL );
  bi_action_repeat_init(repeat,rot);
  bi_add_action(faces[0],repeat);
  bi_action_start(repeat);
  // rotate by on_update
  bi_set_color(faces[1]->color,0,0xFF,0,0xFF);
  bi_node_set_on_update(faces[1],rotate_on_update);
  // rotate by timer
  bi_set_color(faces[2]->color,0,0,0xFF,0xFF);
  BiTimer *t = malloc(sizeof(BiTimer));
  bi_timer_init(t, rotate_on_timer, 0, -1, faces[2]);
  bi_timer_manager_add_timer(&faces[2]->timers,t);
  // rotate by global timer
  t = malloc(sizeof(BiTimer));
  bi_timer_init(t, rotate_on_timer, 0, -1, faces[3]);
  bi_timer_manager_add_timer(&context->timers,t);

  // start
  bi_start_run_loop(context);
  return 0;
}
