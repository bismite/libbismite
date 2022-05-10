#include "common.h"
#include <bi/ext/transition.h>

#define TRANSITION_DURATION 500

BiShader* transition_shader = NULL;
BiLayer *layer_a;
BiLayer *layer_b;
BiLayer *current_layer;

static void end_callback(BiContext* context, BiTransition* transition)
{
  free(transition);
}

static void mid_callback(BiContext* context, BiTransition* transition)
{
  // Dummy
  SDL_Delay(500);

  // clear
  bi_remove_layer(context,current_layer);
  if(current_layer == layer_a){
    current_layer = layer_b;
    bi_add_layer(context,layer_b);
  }else{
    current_layer = layer_a;
    bi_add_layer(context,layer_a);
  }

  // release
  free(transition);

  // transition outro
  BiTransition *transition2 = malloc(sizeof(BiTransition));
  bi_transition_init(transition2,&context->layers,TRANSITION_DURATION,end_callback,transition_shader,true);
  bi_transition_start(context,transition2);
}

static bool on_click_to_transition(BiContext *context,BiNode* n, int x, int y, int button, bool pressed)
{
  if(pressed) return true;
  // transition intro
  BiTransition *transition = malloc(sizeof(BiTransition));
  bi_transition_init(transition,&context->layers,TRANSITION_DURATION,mid_callback,transition_shader,false);
  bi_transition_start(context,transition);
  return true;
}

static void on_update_rotate_face(BiContext* c, BiTimer* t,double dt)
{
  BiNode* n = (BiNode*)t->node;
  bi_node_set_angle(n,n->angle+dt*0.01);
}

static BiLayer* scene_a(BiContext* context)
{
  BiNode* bg = make_sprite("assets/check.png");
  bi_node_set_position(bg,context->w/2,context->h/2);
  BiNode* face = make_sprite("assets/face01.png");
  bi_node_add_node(bg,face);
  onupdate(face,on_update_rotate_face);
  BiLayer *layer = malloc(sizeof(BiLayer));
  bi_layer_init(layer);
  layer->root = bg;
  layer->textures[1] = bg->texture;
  layer->textures[0] = face->texture;
  bi_node_set_on_click(layer->root, on_click_to_transition);
  return layer;
}

static BiLayer* scene_b(BiContext* context)
{
  BiNode* bg = make_sprite("assets/map.png");
  bi_node_set_position(bg,context->w/2,context->h/2);
  BiNode* face = make_sprite("assets/face01.png");
  bi_node_add_node(bg,face);
  onupdate(face,on_update_rotate_face);
  BiLayer *layer = malloc(sizeof(BiLayer));
  bi_layer_init(layer);
  layer->root = bg;
  layer->textures[1] = bg->texture;
  layer->textures[0] = face->texture;
  bi_node_set_on_click(layer->root, on_click_to_transition);
  return layer;
}

int main(int argc, char* argv[])
{
  BiContext* context = malloc(sizeof(BiContext));
  bi_init_context(context, 480, 320, 0, true, __FILE__);

  transition_shader = create_shader_with_default_vertex_shader("assets/shaders/transition.frag");

  layer_a = scene_a(context);
  layer_b = scene_b(context);
  current_layer = layer_a;
  bi_add_layer(context,layer_a);

  bi_start_run_loop(context);
  return 0;
}
