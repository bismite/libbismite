#include "common.h"

static bool on_click(BiContext* context,BiNode* n, int x, int y, int button, bool pressed)
{
  if(pressed) {
    bi_node_set_angle(n,n->angle + 30*3.1415/180.0);
    bi_node_set_position(n,x,y);
  }
  return true;
}

static bool on_move_cursor(BiContext* context,BiNode* n, int x, int y)
{
    bi_node_set_position(n,x,y);
    return true;
}

#ifdef __EMSCRIPTEN__
static bool on_touch(BiContext* context,BiNode* n, float x, float y, int64_t finger_id, bool pressed)
{
  if(pressed) {
    bi_node_set_position(n,x*480,y*320);
    bi_node_set_angle(n,n->angle + 30*3.1415/180.0);
  }
  return true;
}

static bool on_move_finger(BiContext* context,BiNode* n, float x, float y, int64_t finger_id)
{
    bi_node_set_position(n,x*480, y * 320);
    return true;
}
#endif

int main(int argc,char* argv[])
{
    BiContext* context = malloc(sizeof(BiContext));
    bi_init_context(context, 480, 320, 0, false, __FILE__);
    print_info(context);

    // root node
    BiNode* root = malloc(sizeof(BiNode));
    bi_node_init(root);

    // sprite
    BiNode* face = make_sprite("assets/face01.png");
    bi_node_set_position(face,context->w/2,context->h/2);
    bi_node_add_node(root,face);

    // set callbacks
    bi_node_set_on_click(face, on_click);
    bi_node_set_on_move_cursor(face, on_move_cursor);
#ifdef __EMSCRIPTEN__
    bi_node_set_on_move_finger(face, on_move_finger);
    bi_node_set_on_touch(face, on_touch);
#endif

    // layer
    BiLayer *layer = malloc(sizeof(BiLayer));
    bi_layer_init(layer);
    bi_add_layer(context,layer);
    layer->root = root;
    layer->textures[0] = face->texture;

    // fps layer
    add_fps_layer(context,load_font());

    bi_start_run_loop(context);
    return 0;
}
