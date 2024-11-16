#include <bi/shader_node.h>
#include <bi/node.h>
#include <bi/render.h>
#include <bi/context.h>

BiShaderNode* bi_shader_node_init(BiShaderNode* shader_node)
{
  bi_node_base_init( (BiNodeBase*)shader_node, BI_SHADER_NODE );
  shader_node->blend_factor = BI_BLEND_FACTOR_DEFAULT;

  shader_node->camera_x = 0;
  shader_node->camera_y = 0;
  for(int i=0;i<BI_SHADER_MAX_TEXTURES;i++) {
    shader_node->textures[i] = NULL;
  }
  shader_node->shader = NULL;
  for(int i=0;i<16;i++) {
    shader_node->shader_extra_data[i] = 0;
  }
  //
  return shader_node;
}

// Timer
BiTimer* bi_shader_node_add_timer(BiShaderNode* shader_node,BiTimer* timer){
  return bi_timers_add( &shader_node->timers,timer);
}
BiTimer* bi_shader_node_remove_timer(BiShaderNode* shader_node,BiTimer* timer){
  return bi_timers_remove( &shader_node->timers,timer);
}
