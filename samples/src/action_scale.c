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

  // layer
  BiLayer *layer = bi_layer_init(ALLOC(BiLayer));
  layer->root = make_sprite_with_anchor("assets/check.png",0,0);
  bi_add_layer(context,layer);

  // sprite
  BiNode* face = make_sprite("assets/face01.png");
  bi_node_set_position(face,240,160);
  bi_node_add_node(layer->root,face);
  layer->textures[0] = layer->root->texture;
  layer->textures[1] = face->texture;

  // x0.5
  bi_node_set_scale(face,0.5,0.5);
  // Scale
  BiActionScale* scale = bi_action_scale_by_init(ALLOC(BiActionScale),500,4,4);
  bi_add_action(face,(BiAction*)scale);
  bi_action_start((BiAction*)scale);
  scale->action.on_finish = callback;

  bi_start_run_loop(context);
  return 0;
}
