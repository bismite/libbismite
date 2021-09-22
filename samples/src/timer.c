#include "common.h"

static bool rotate_face(BiContext* context,BiTimer* timer)
{
  BiNode *node = timer->userdata;
  // LOG("rotate_face %lld\n", now);
  bi_node_set_angle(node, node->angle + 1 * M_PI/180);
  return true;
}

static bool global_timer_funciton(BiContext *context,BiTimer* timer)
{
  static int counter = 0;
  LOG("this is global timer function: %ld\n", (long)context->frame_start_at);
  BiNode *node = timer->userdata;
  bi_node_set_scale(node,node->scale_x*1.2,node->scale_y*1.2);
  counter++;
  if(counter>2){
    LOG("timer sucide\n");
    bi_remove_timer(&context->timers,timer);
    free(timer);
  }
  return true;
}

static bool global_timer_funciton2(BiContext* context,BiTimer* timer)
{
  BiNode *node = timer->userdata;
  bi_set_color( node->color, rand()%0xFF, rand()%0xFF, rand()%0xFF, 0xFF);
  LOG("change color: %ld\n", (long)context->frame_start_at);
  return true;
}

int main(int argc,char* argv[])
{
  BiContext* context = malloc(sizeof(BiContext));
  bi_init_context(context, 480, 320, 0, false, __FILE__);
  print_info(context);

  // root node
  BiNode* root = make_sprite("assets/check.png");
  bi_node_set_position(root,context->w/2,context->h/2);

  // sprite node
  BiNode* face = make_sprite("assets/face01.png");
  bi_node_add_node(root,face);

  // add global timer
  BiTimer *global_timer = malloc(sizeof(BiTimer));
  bi_timer_init(global_timer, global_timer_funciton, 1000, -1, face);
  bi_add_timer(&context->timers,global_timer);

  // add global timer2
  BiTimer *global_timer2 = malloc(sizeof(BiTimer));
  bi_timer_init(global_timer2, global_timer_funciton2, 500, -1, face);
  bi_add_timer(&context->timers,global_timer2);

  // add timer
  BiTimer *timer = malloc(sizeof(BiTimer));
  bi_timer_init(timer, rotate_face, 10, -1, face);
  bi_add_timer(&face->timers,timer);

  // layer
  BiLayer *layer = malloc(sizeof(BiLayer));
  bi_layer_init(layer);
  layer->textures[0] = root->texture_mapping->texture;
  layer->textures[1] = face->texture_mapping->texture;
  bi_add_layer(context,layer);
  layer->root = root;

  // fps layer
  BiFontAtlas *font = load_font();
  add_fps_layer(context,font);

  bi_start_run_loop(context);
  return 0;
}
