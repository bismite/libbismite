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
  BiContext* context = bi_init_context(ALLOC(BiContext),480,320,0,false,__FILE__);
  print_info(context);

#ifdef EMSCRIPTEN
  emscripten_set_visibilitychange_callback(context,true,visibilitychange_callback);
#endif

  layer = bi_layer_init(ALLOC(BiLayer));
  layer->root = make_sprite_with_anchor("assets/check.png",0,0);
  layer->textures[0] = layer->root->texture;
  bi_add_layer(context,layer);

  bi_start_run_loop(context);
  return 0;
}
