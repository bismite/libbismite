#include "common.h"

static void rotate_face(BiContext* context,BiTimer* timer,double dt)
{
  BiNode *node = timer->userdata;
  bi_node_set_angle(node, node->angle + 1 * M_PI/180);
}

static void pause_rotate(BiContext* context,BiTimer* timer,double dt)
{
  BiTimer* timer_rotate_face = timer->userdata;
  bi_timer_pause(timer_rotate_face);
  printf("pause_rotate : %lld\n", (long long)bi_get_now());
  if(timer->count==0){
    printf("remove timer pause_rotate\n");
    bi_timer_manager_remove_timer(timer->manager,timer);
    free(timer);
  }
}
static void resume_rotate(BiContext* context,BiTimer* timer,double dt)
{
  BiTimer* timer_rotate_face = timer->userdata;
  bi_timer_resume(timer_rotate_face);
  printf("resume_rotate : %lld\n", (long long)bi_get_now());
  if(timer->count==0){
    printf("remove timer resume_rotate\n");
    bi_timer_manager_remove_timer(timer->manager,timer);
    free(timer);
  }
}

static void enlarge(BiContext *context,BiTimer* timer,double dt)
{
  static int counter = 0;
  BiNode *node = timer->userdata;
  bi_node_set_scale(node,node->scale_x*1.2,node->scale_y*1.2);
  printf("%d: scale:%f\n", counter, node->scale_x);
  counter++;
  if(counter>2){
    printf("enlarge timer sucide\n");
    bi_timer_manager_remove_timer(timer->manager,timer);
    free(timer);
  }
}

static void change_color(BiContext* context,BiTimer* timer,double dt)
{
  BiNode *node = timer->userdata;
  bi_set_color( node->color, rand()%0xFF, rand()%0xFF, rand()%0xFF, 0xFF);
  printf("change color\n");
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
  bi_timer_init(global_timer, enlarge, 1000, -1, face);
  bi_timer_manager_add_timer(&context->timers,global_timer);

  // add global timer2
  BiTimer *global_timer2 = malloc(sizeof(BiTimer));
  bi_timer_init(global_timer2, change_color, 500, 3, face);
  bi_timer_manager_add_timer(&context->timers,global_timer2);

  // rotate
  BiTimer *timer_rotate_face = malloc(sizeof(BiTimer));
  bi_timer_init(timer_rotate_face, rotate_face, 10, -1, face);
  bi_timer_manager_add_timer(&face->timers,timer_rotate_face);
  // pause rotate
  BiTimer *timer = malloc(sizeof(BiTimer));
  bi_timer_init(timer, pause_rotate, 1000, 1, timer_rotate_face);
  bi_timer_manager_add_timer(&face->timers,timer);
  // resume rotate
  timer = malloc(sizeof(BiTimer));
  bi_timer_init(timer, resume_rotate, 2000, 1, timer_rotate_face);
  bi_timer_manager_add_timer(&face->timers,timer);

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
