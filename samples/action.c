#include "common.h"
#include <bi/ext/action.h>

int main(int argc, char* argv[])
{
  BiContext* context = make_context(__FILE__);

  // layer
  BiLayer *layer = bi_layer_init(ALLOC(BiLayer));
  BiNode* root = bi_layer_add_node(layer, make_bg("assets/check.png") );
  bi_add_layer(context,layer);

  // sprite
  BiNode* face = make_sprite("assets/face01.png");
  bi_node_set_position(face,240,160);
  bi_node_add_node(root,face);
  layer->textures[0] = root->texture;
  layer->textures[1] = face->texture;

  // action
  BiActionRotate* rot = bi_action_rotate_by_init(ALLOC(BiActionRotate),1000,360);
  bi_action_set_repeat((BiAction*)rot, 3 );
  bi_add_action(face,(BiAction*)rot);
  bi_action_start((BiAction*)rot);

  bi_start_run_loop(context);
  return 0;
}
