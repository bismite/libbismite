#include "common.h"
#include <bi/ext/action.h>

BiNode *background=NULL;

#ifdef EMSCRIPTEN
#include <emscripten/html5.h>
EM_BOOL visibilitychange_callback(int eventType,
                                  const EmscriptenVisibilityChangeEvent *visibilityChangeEvent,
                                  void *userData)
{
  printf("visibilitychange_callback\n");
  if(background) {
    background->tint = RGBA32(0xff00ff99);
  }
  return EM_TRUE;
}
#endif

int main(int argc, char* argv[])
{
  BiContext* context = make_context(__FILE__);

#ifdef EMSCRIPTEN
  emscripten_set_visibilitychange_callback(context,true,visibilitychange_callback);
#endif

  BiLayer* layer = bi_layer_init(ALLOC(BiLayer));
  background = bi_layer_add_node(layer,make_bg("assets/check.png"));
  layer->textures[0] = background->texture;
  bi_add_layer(context,layer);

  bi_start_run_loop(context);
  return 0;
}
