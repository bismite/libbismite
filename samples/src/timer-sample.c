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
    bi_raw_node_remove_timer(timer->node,timer);
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
    bi_raw_node_remove_timer(timer->node,timer);
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
    bi_raw_node_remove_timer(timer->node,timer);
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
  BiContext* context = bi_init_context(malloc(sizeof(BiContext)),480,320,0,false,__FILE__);
  print_info(context);

  // layer
  BiLayer *layer = bi_layer_init(malloc(sizeof(BiLayer)));
  bi_add_layer(context,layer);
  layer->root = make_sprite_with_anchor("assets/check.png",0,0);;
  BiNode* face = make_sprite("assets/face01.png");
  bi_node_set_position(face,context->w/2,context->h/2);
  bi_node_add_node(layer->root,face);
  layer->textures[0] = layer->root->texture;
  layer->textures[1] = face->texture;

  // timer on layer group
  bi_layer_group_add_timer(&context->layers,bi_timer_init(malloc(sizeof(BiTimer)),enlarge,1000,-1,face));
  // timer on layer
  bi_layer_add_timer(layer,bi_timer_init(malloc(sizeof(BiTimer)),change_color,500,3,face));

  // rotate
  BiTimer *rotate_timer = bi_timer_init(malloc(sizeof(BiTimer)),rotate,10,-1,NULL);
  bi_node_add_timer(face,rotate_timer);
  // pause rotate
  bi_node_add_timer(face,bi_timer_init(malloc(sizeof(BiTimer)),pause_rotate,1000,2,rotate_timer));
  // resume rotate
  bi_node_add_timer(face,bi_timer_init(malloc(sizeof(BiTimer)),resume_rotate,1500,2,rotate_timer));

  // fps layer
  BiFontAtlas *font = load_font();
  add_fps_layer(context,font);

  bi_start_run_loop(context);
  return 0;
}
