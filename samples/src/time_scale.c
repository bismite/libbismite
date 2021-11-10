#include "common.h"

static void rotate_face(BiContext* context,BiTimer* timer,double dt)
{
  BiNode *node = timer->userdata;
  bi_node_set_angle(node, node->angle + dt * M_PI/180);
}

int main(int argc,char* argv[])
{
  BiContext* context = malloc(sizeof(BiContext));
  bi_init_context(context, 480, 320, 0, false, __FILE__);
  print_info(context);

  // root node
  BiNode* root = make_sprite("assets/check.png");
  bi_node_set_position(root,0,0);
  bi_node_set_anchor(root,0,0);

  // sprites
  float TIMESCALES[3] = {2.0, 1.0, 0.5};
  BiTextureMapping* m = make_texture_mapping("assets/face01.png");
  for(int i=0;i<3;i++){
    BiNode* face = make_sprite_from_mapping(m);
    bi_node_add_node(root,face);
    bi_node_set_position(face,i*120+120,160);
    BiTimer *timer_rotate_face = malloc(sizeof(BiTimer));
    bi_timer_init(timer_rotate_face, rotate_face, 0, -1, face);
    bi_timer_manager_add_timer(&face->timers,timer_rotate_face);
    face->time_scale = TIMESCALES[i];
  }

  // layer
  BiLayer *layer = malloc(sizeof(BiLayer));
  bi_layer_init(layer);
  layer->textures[0] = root->texture_mapping->texture;
  layer->textures[1] = m->texture;
  bi_add_layer(context,layer);
  layer->root = root;

  // fps layer
  BiFontAtlas *font = load_font();
  add_fps_layer(context,font);

  bi_start_run_loop(context);
  return 0;
}
