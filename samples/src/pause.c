#include "common.h"
#include <bi/ext/action.h>

BiLayerGroup* layer_group_a;
BiLayerGroup* layer_group_b;

static BiAction* alloc_action(size_t size, void* payload)
{
  BiAction* action = malloc(sizeof(BiAction));
  bi_action_init(action);
  action->action_data = malloc(size);
  action->on_finish = NULL;
  action->on_finish_callback_context = payload;
  return action;
}

static void rotate_on_timer(BiContext* context,BiTimer* timer,double dt)
{
  BiNode *node = timer->userdata;
  bi_node_set_angle(node, node->angle + 2.0*M_PI/180.0);
}

static bool on_click(BiContext* context,BiNode* n, int x, int y, int button, bool pressed)
{
  if(pressed){
    layer_group_a->time_scale = layer_group_a->time_scale == 0 ? 1.0 : 0;
  }
  return true; // swallow
}

static void init_layer_group(BiContext* context, BiLayerGroup* lg, int offset_y)
{
  bi_layer_group_init(lg);
  bi_layer_group_add_layer_group(&context->layers, lg);

  // root node
  BiNode* root = bi_node_init(malloc(sizeof(BiNode)));

  // sprite
  BiTextureMapping* tm = make_texture_mapping("assets/face01.png");
  BiNode* faces[2];
  for(int i=0;i<2;i++){
    BiNode* face = make_sprite_from_mapping(tm);
    bi_node_set_position(face,480/3*(1+i%2),offset_y);
    bi_node_add_node(root,face);
    faces[i] = face;
  }

  // layer
  BiLayer *layer = bi_layer_init(malloc(sizeof(BiLayer)));
  layer->root = root;
  bi_layer_group_add_layer(lg,layer);
  layer->textures[0] = tm->texture;

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
  // rotate by timer
  bi_set_color(faces[1]->color,0,0,0xFF,0xFF);
  BiTimer *t = malloc(sizeof(BiTimer));
  bi_timer_init(t, rotate_on_timer, 0, -1, faces[1]);
  bi_node_add_timer(faces[1],t);
}

int main(int argc, char* argv[])
{
  BiContext* context = malloc(sizeof(BiContext));
  bi_init_context(context, 480, 320, 60, false, __FILE__);
  print_info(context);

  // bg
  BiNode* bg = make_sprite("assets/check.png");
  bi_node_set_position(bg,context->w/2,context->h/2);
  // layer
  BiLayer *layer = bi_layer_init(malloc(sizeof(BiLayer)));
  layer->root = bg;
  bi_layer_group_add_layer(&context->layers,layer);
  layer->textures[0] = bg->texture_mapping->texture;

  layer_group_a = malloc(sizeof(BiLayerGroup));
  init_layer_group(context, layer_group_a, 320/3*2);
  layer_group_b = malloc(sizeof(BiLayerGroup));
  init_layer_group(context, layer_group_b, 320/3*1);

  //
  // fps layer
  //
  BiFontAtlas *font = load_font();
  add_fps_layer(context,font);

  // start
  bi_start_run_loop(context);
  return 0;
}
