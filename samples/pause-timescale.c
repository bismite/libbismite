#include "common.h"
#include <bi/ext/action.h>

BiLayerGroup* layer_group_a;
BiLayerGroup* layer_group_b;

static void rotate_on_timer(BiTimer* timer,double dt)
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

static BiLayerGroup* make_layer_group(BiContext* context, BiLayerGroup* lg, int offset_y)
{
  bi_layer_group_init(lg);
  bi_layer_group_add_layer_group(&context->layers, lg);
  // layer
  BiTexture* tex = MAKE_TEXTURE("assets/face01.png");
  BiLayer *layer = bi_layer_init(ALLOC(BiLayer));
  BiNode* root = &layer->root;
  bi_layer_group_add_layer(lg,layer);
  layer->textures[0] = tex;
  // sprite
  BiNode* faces[2];
  for(int i=0;i<2;i++){
    BiNode* face = make_sprite_from_texture(tex);
    bi_node_set_position(face,480/3*(1+i%2),offset_y);
    bi_node_add_node(root,face);
    faces[i] = face;
  }
  // rotate by action
  faces[0]->color_tint = RGBA(0xFF,0,0,0xFF);
  BiAction* rot = (BiAction*)bi_action_rotate_by_init(ALLOC(BiActionRotate),0,2);
  bi_action_set_repeat(rot,-1);
  bi_add_action(faces[0],rot);
  bi_action_start(rot);
  // rotate by timer
  faces[1]->color_tint = RGBA(0,0,0xFF,0xFF);
  bi_node_add_timer(faces[1], bi_timer_init(ALLOC(BiTimer), rotate_on_timer, 0, -1, faces[1]));
  return lg;
}

int main(int argc, char* argv[])
{
  BiContext* context = make_context(__FILE__);
  // layer
  BiLayer *layer = bi_layer_init(ALLOC(BiLayer));
  set_texture(&layer->root,"assets/check.png");
  bi_node_set_on_click(&layer->root, on_click);
  bi_layer_group_add_layer(&context->layers,layer);
  layer->textures[0] = layer->root.texture;
  layer_group_a = make_layer_group(context, ALLOC(BiLayerGroup), 320/3*2);
  layer_group_b = make_layer_group(context, ALLOC(BiLayerGroup), 320/3*1);
  // start
  bi_start_run_loop(context);
  return 0;
}
