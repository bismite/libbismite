#include "common.h"
#include <bi/ext/action.h>

BiLayer *layer=NULL;

#ifdef EMSCRIPTEN
#include <emscripten/html5.h>
EM_BOOL visibilitychange_callback(int eventType,
                                  const EmscriptenVisibilityChangeEvent *visibilityChangeEvent,
                                  void *userData)
{
  printf("visibilitychange_callback\n");
  if(layer) bi_set_color(layer->root->color_tint, 0xff,0,0xff,64);
  return EM_TRUE;
}
#endif

int main(int argc, char* argv[])
{
  BiContext* context = make_context(__FILE__);

#ifdef EMSCRIPTEN
  emscripten_set_visibilitychange_callback(context,true,visibilitychange_callback);
#endif

  layer = bi_layer_init(ALLOC(BiLayer));
  set_texture(&layer->root, "assets/check.png");
  layer->textures[0] = layer->root.texture;
  bi_add_layer(context,layer);

  bi_start_run_loop(context);
  return 0;
}
