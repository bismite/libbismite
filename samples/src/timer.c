#include "common.h"

static void rotate(BiContext* context,BiTimer* timer,double dt)
{
  BiNode *node = (BiNode*)timer->node;
  bi_node_set_angle(node, node->angle + dt*0.01);
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

  // layer
  BiLayer *layer = malloc(sizeof(BiLayer));
  bi_layer_init(layer);
  layer->textures[0] = root->texture_mapping->texture;
  layer->textures[1] = face->texture_mapping->texture;
  bi_add_layer(context,layer);
  layer->root = root;

  // timer on layer group
  bi_node_add_timer(&context->layers,bi_timer_init(malloc(sizeof(BiTimer)),enlarge,1000,-1,face));
  // timer on layer
  bi_node_add_timer(layer,bi_timer_init(malloc(sizeof(BiTimer)),change_color,500,3,face));

  // rotate
  BiTimer *rotate_timer = bi_timer_init(malloc(sizeof(BiTimer)),rotate,10,-1,NULL);
  bi_node_add_timer(face,rotate_timer);
  // pause rotate
  bi_node_add_timer(face,bi_timer_init(malloc(sizeof(BiTimer)),pause_rotate,1000,1,rotate_timer));
  // resume rotate
  bi_node_add_timer(face,bi_timer_init(malloc(sizeof(BiTimer)),resume_rotate,2000,1,rotate_timer));

  // fps layer
  BiFontAtlas *font = load_font();
  add_fps_layer(context,font);

  bi_start_run_loop(context);
  return 0;
}
