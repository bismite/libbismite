#include "common.h"

static void rotate_face(BiContext* context,BiTimer* timer,double dt)
{
  BiNode *node = (BiNode*)timer->node;
  bi_node_set_angle(node, node->angle + dt * M_PI/180);
}

int main(int argc,char* argv[])
{
  BiContext* context = bi_init_context(malloc(sizeof(BiContext)),480,320,0,false,__FILE__);
  print_info(context);

  // layer
  BiLayer *layer = bi_layer_init(malloc(sizeof(BiLayer)));
  bi_add_layer(context,layer);
  layer->root = make_sprite_with_anchor("assets/check.png",0,0);

  // sprites
  float TIMESCALES[4] = {2.0, 1.0, 0.5, 0};
  BiTexture* tex = MAKE_TEXTURE("assets/face01.png");
  for(int i=0;i<4;i++){
    BiNode* face = make_sprite_from_texture(tex);
    bi_node_add_node(layer->root,face);
    bi_node_set_position(face,i*480/5+480/5,160);
    BiTimer *timer_rotate_face = ALLOC(BiTimer);
    bi_timer_init(timer_rotate_face, rotate_face, 0, -1, NULL);
    bi_node_add_timer(face,timer_rotate_face);
    face->time_scale = TIMESCALES[i];
  }

  // textures
  layer->textures[0] = layer->root->texture;
  layer->textures[1] = tex;

  // fps layer
  add_fps_layer(context,load_font());

  bi_start_run_loop(context);
  return 0;
}
