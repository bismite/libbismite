#include "common.h"
#include <bi/ext/transition.h>

#define TRANSITION_DURATION 500

BiShader* transition_shader = NULL;

typedef void (*scene_builder)(BiContext*);
static void scene_a(BiContext* context);
static void scene_b(BiContext* context);
static bool on_click_to_transition(BiContext *context,BiNode* n, int x, int y, int button, bool pressed);

static void on_update_rotate_face(BiContext* c, BiNode* n)
{
  bi_node_set_angle(n,n->angle+0.1);
}

static void scene_b(BiContext* context)
{
  BiNode* bg = make_sprite("assets/check.png");
  bi_node_set_position(bg,context->w/2,context->h/2);
  BiNode* face = make_sprite("assets/face01.png");
  bi_node_add_node(bg,face);
  BiLayer *layer = malloc(sizeof(BiLayer));
  bi_layer_init(layer);
  bi_add_layer(context,layer);
  layer->root = bg;
  layer->textures[1] = bg->texture_mapping->texture;
  layer->textures[0] = face->texture_mapping->texture;
  //
  bi_node_set_on_update(face,on_update_rotate_face);
  //
  layer->root->userdata = scene_a;
  bi_node_set_on_click(layer->root, on_click_to_transition);
}

static void transition_end_callback(BiContext* context, BiTransition* transition)
{
  free(transition);
}

static void transition_mid_callback(BiContext* context, BiTransition* transition)
{
  // Dummy
  SDL_Delay(500);

  // clear
  bi_remove_layer(context,context->layers.layers.objects[0]);

  // new scene
  scene_builder f = transition->userdata;
  f(context);

  // release
  free(transition);

  // transition outro
  BiTransition *transition2 = malloc(sizeof(BiTransition));
  bi_transition_init(transition2,&context->layers,TRANSITION_DURATION,transition_end_callback,transition_shader,true);
  bi_transition_start(context,transition2);
}

static bool on_click_to_transition(BiContext *context,BiNode* n, int x, int y, int button, bool pressed)
{
  if(pressed) return true;
  // transition intro
  BiTransition *transition = malloc(sizeof(BiTransition));
  bi_transition_init(transition,&context->layers,TRANSITION_DURATION,transition_mid_callback,transition_shader,false);
  transition->userdata = n->userdata;
  bi_transition_start(context,transition);
  return true;
}

static void scene_a(BiContext* context)
{
  BiNode* bg = make_sprite("assets/check.png");
  bi_node_set_position(bg,context->w/2,context->h/2);
  bi_set_color(bg->color,0x80,0x80,0,0xff);
  BiNode* face = make_sprite("assets/face01.png");
  bi_node_add_node(bg,face);
  BiLayer *layer = malloc(sizeof(BiLayer));
  bi_layer_init(layer);
  bi_add_layer(context,layer);
  layer->root = bg;
  layer->textures[1] = bg->texture_mapping->texture;
  layer->textures[0] = face->texture_mapping->texture;
  //
  bi_node_set_on_update(face,on_update_rotate_face);
  //
  layer->root->userdata = scene_b;
  bi_node_set_on_click(layer->root, on_click_to_transition);
}

int main(int argc, char* argv[])
{
  BiContext* context = malloc(sizeof(BiContext));
  bi_init_context(context, 480, 320, 0, true, __FILE__);

  transition_shader = create_shader("assets/shaders/default.vert","assets/shaders/transition.frag");

  scene_a(context);

  bi_start_run_loop(context);
  return 0;
}
