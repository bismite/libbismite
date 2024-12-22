#include "common.h"

int main(int argc, char* argv[])
{
  // BiContext* context = make_context(__FILE__);
  srand( bi_get_now() );
  uint32_t flags = BI_WINDOW_ALLOW_HIGHDPI | BI_WINDOW_RESIZABLE;
  BiContext* context = bi_init_context(ALLOC(BiContext),W,H,10,flags,__FILE__);
  print_info(context);
  context->window_event_callback[SDL_WINDOWEVENT_SIZE_CHANGED] = on_window_size_changed;

  // Framebuffer Node
  BiNode* framebuffer_node = bi_node_init(ALLOC(BiNode));
  bi_node_set_size(framebuffer_node,W,H);
  framebuffer_node->framebuffer = bi_framebuffer_init_with_texture_num(ALLOC(BiFramebuffer),W,H,2);
  bi_node_set_texture(framebuffer_node, &framebuffer_node->framebuffer->textures[0],0,0,W,H);

  // Shader1 : Render to texture[1]
  BiShaderNode *shader_node1 = bi_shader_node_init(ALLOC(BiShaderNode));
  shader_node1->shader = create_shader_with_default_vertex_shader("assets/shaders/mrt-simple.frag");
  shader_node1->shader->output[0]=false;
  shader_node1->shader->output[1]=true;
  BiNode* rect = bi_node_init(ALLOC(BiNode));
  bi_node_set_size(rect,100,100);
  bi_node_set_position(rect, 100,100);
  rect->color = RGBA(0xff,0,0xff,0xff);
  bi_node_add_node(shader_node1, rect);
  bi_node_add_node(framebuffer_node, shader_node1);

  // Shader2: texture[1] -> texture[0]
  BiShaderNode *shader_node2 = bi_shader_node_init(ALLOC(BiShaderNode));
  shader_node2->shader =  create_shader_with_default_vertex_shader("assets/shaders/mrt-simple-result.frag");
  BiNode* board = bi_node_init(ALLOC(BiNode));
  bi_node_set_texture(board, &framebuffer_node->framebuffer->textures[1],0,0,W,H);
  shader_node2->textures[0] = &framebuffer_node->framebuffer->textures[1];
  bi_node_set_size(board,W,H);
  bi_node_add_node(shader_node2, board);
  bi_node_add_node(framebuffer_node, shader_node2);

  // Main Layer
  BiShaderNode *shader_node_result = bi_shader_node_init(ALLOC(BiShaderNode));
  bi_node_add_node(shader_node_result, framebuffer_node);
  shader_node_result->textures[0] = bi_node_get_texture(framebuffer_node);
  bi_node_add_node(&context->default_framebuffer_node,shader_node_result);

  // start
  bi_start_run_loop(context);
  return 0;
}
