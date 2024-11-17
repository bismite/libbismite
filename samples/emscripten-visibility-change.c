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

  BiShaderNode* shader_node = bi_shader_node_init(ALLOC(BiShaderNode));
  background = bi_node_add_node(shader_node,make_bg("assets/check.png"));
  shader_node->textures[0] = bi_node_get_texture(background);
  bi_node_add_node(&context->default_framebuffer_node,shader_node);

  bi_start_run_loop(context);
  return 0;
}
