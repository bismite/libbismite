#include "common.h"
#include <bi/ext/action.h>

static void callback(BiAction* action,void* context)
{
  printf("Scale %f,%f\n",action->node->scale_x, action->node->scale_y);
}

int main(int argc, char* argv[])
{
  BiContext* context = bi_init_context(ALLOC(BiContext),480,320,0,false,__FILE__);
  print_info(context);

  // sprite
  BiTexture *texture = MAKE_TEXTURE("assets/face01.png");
  // layer
  BiLayer *layer = bi_layer_init(ALLOC(BiLayer));
  layer->root = make_sprite_with_anchor("assets/check.png",0,0);
  bi_add_layer(context,layer);
  layer->textures[0] = layer->root->texture;
  layer->textures[1] = texture;

  // face green
  BiNode* face_green = make_sprite_from_texture(texture);
  bi_set_color(face_green->color, 0,0xff,0,0xff);
  bi_node_set_position(face_green,240,160);
  bi_node_add_node(layer->root,face_green);
  // face red
  BiNode* face_red = make_sprite_from_texture(texture);
  bi_set_color(face_red->color, 0xff,0,0,0xff);
  bi_node_set_position(face_red,120,80);
  bi_node_add_node(layer->root,face_red);
  bi_node_set_scale(face_red,0.5,0.5); // x0.5
  // face blue
  BiNode* face_blue = make_sprite_from_texture(texture);
  bi_set_color(face_blue->color, 0,0,0xff,0xff);
  bi_node_set_position(face_blue,360,240);
  bi_node_add_node(layer->root,face_blue);
  bi_node_set_scale(face_blue,0.5,0.5); // x0.5

  // Scale By
  BiActionScale* scale_by = bi_action_scale_by_init(ALLOC(BiActionScale),5000,2,2);
  scale_by->action.on_finish = callback;
  bi_add_action(face_red,(BiAction*)scale_by);
  bi_action_start((BiAction*)scale_by);

  // Scale To
  BiActionScale* scale_to = bi_action_scale_to_init(ALLOC(BiActionScale),5000,2,2);
  scale_to->action.on_finish = callback;
  bi_add_action(face_blue,(BiAction*)scale_to);
  bi_action_start((BiAction*)scale_to);

  bi_start_run_loop(context);
  return 0;
}
