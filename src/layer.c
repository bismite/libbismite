#include <bi/shader_node.h>
#include <bi/node.h>
#include <bi/render.h>
#include <bi/context.h>

extern void render_postprocess(BiContext* context,
                               BiNodeBase *shader_node,
                               BiFramebuffer* dst,
                               BiRenderingContext rc
                             );
extern void render_shader_node(BiContext* context,
                         BiNodeBase *shader_node,
                         BiFramebuffer *dst,
                         BiRenderingContext rc
                       );
//
// Layer Group
//

BiFramebufferNode* bi_framebuffer_node_init_with_size(BiFramebufferNode* framebuffer_node,int w, int h)
{
  bi_node_base_init((BiNodeBase*)framebuffer_node,BI_LAYER_GROUP);
  framebuffer_node->blend_factor = BI_BLEND_FACTOR_DEFAULT;
  bi_framebuffer_init(&framebuffer_node->framebuffer,w,h);
  framebuffer_node->w = w;
  framebuffer_node->h = h;
  return framebuffer_node;
}

BiFramebufferNode* bi_framebuffer_node_init(BiFramebufferNode* framebuffer_node)
{
  GLint dims[4] = {0};
  glGetIntegerv(GL_VIEWPORT, dims);
  return bi_framebuffer_node_init_with_size(framebuffer_node,dims[2],dims[3]);
}

int bi_framebuffer_node_get_z_order(BiFramebufferNode* framebuffer_node)
{
  return framebuffer_node->z;
}

void bi_framebuffer_node_set_z_order(BiFramebufferNode* framebuffer_node,int z)
{
  framebuffer_node->z = z;
}

BiShaderNode* bi_framebuffer_node_add_shader_node(BiFramebufferNode* framebuffer_node, BiShaderNode* obj)
{
  obj->parent = (BiNodeBase*)framebuffer_node;
  return array_add_object(&framebuffer_node->children,obj);
}

BiShaderNode* bi_framebuffer_node_remove_shader_node(BiFramebufferNode* framebuffer_node, BiShaderNode* obj)
{
  if( obj && obj == array_remove_object(&framebuffer_node->children,obj) ){
    obj->parent = NULL;
    return obj;
  }
  return NULL;
}

BiFramebufferNode* bi_framebuffer_node_add_framebuffer_node(BiFramebufferNode* framebuffer_node, BiFramebufferNode* obj)
{
  obj->parent = (BiNodeBase*)framebuffer_node;
  return array_add_object(&framebuffer_node->children,obj);
}

BiFramebufferNode* bi_framebuffer_node_remove_framebuffer_node(BiFramebufferNode* framebuffer_node, BiFramebufferNode* obj)
{
  if( obj && obj == array_remove_object(&framebuffer_node->children,obj) ){
    obj->parent = NULL;
    return obj;
  }
  return NULL;
}

//
// LayerGroup Draw
//
void bi_framebuffer_node_clear(BiFramebufferNode* canvas,uint8_t r,uint8_t g,uint8_t b,uint8_t a)
{
  glBindFramebuffer(GL_FRAMEBUFFER, canvas->framebuffer.framebuffer_id);
  glClearColor(r/255.0, g/255.0, b/255.0, a/255.0);
  glClear(GL_COLOR_BUFFER_BIT);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void bi_framebuffer_node_draw(BiFramebufferNode* canvas, BiContext* context)
{
  // viewport setting
  GLint tmp[4];
  glGetIntegerv(GL_VIEWPORT,tmp);
  glViewport(0,0,canvas->w,canvas->h);
  // camera
  int _w = context->w;
  int _h = context->h;
  context->w = canvas->w;
  context->h = canvas->h;
  // rendering
  BiRenderingContext rendering_context;
  bi_rendering_context_init(&rendering_context,true,true,1.0,
                            NULL, //&context->interaction_queue,
                            NULL, // &context->timer_queue,
                            &context->rendering_queue);
  bi_render_framebuffer_node( context, (BiNodeBase*)canvas, rendering_context );
  // restore
  context->w = _w;
  context->h = _h;
  // restore viewport
  glViewport(tmp[0],tmp[1],tmp[2],tmp[3]);
}

//
// Layer
//

BiShaderNode* bi_shader_node_init(BiShaderNode* shader_node)
{
  bi_node_base_init( (BiNodeBase*)shader_node, BI_LAYER );
  shader_node->_render_function_ = render_shader_node;
  shader_node->blend_factor = BI_BLEND_FACTOR_DEFAULT;

  shader_node->camera_x = 0;
  shader_node->camera_y = 0;
  for(int i=0;i<BI_LAYER_MAX_TEXTURES;i++) {
    shader_node->textures[i] = NULL;
  }
  shader_node->shader = NULL;
  for(int i=0;i<16;i++) {
    shader_node->shader_extra_data[i] = 0;
  }
  //
  return shader_node;
}

BiShaderNode* bi_shader_node_init_as_postprocess(BiShaderNode* shader_node)
{
  bi_shader_node_init(shader_node);
  shader_node->_render_function_ = render_postprocess;
  return shader_node;
}

BiShaderNode* bi_shader_node_remove_from_parent(BiShaderNode* shader_node)
{
  if(shader_node && shader_node->parent) return bi_framebuffer_node_remove_shader_node((BiFramebufferNode*)shader_node->parent,shader_node);
  return NULL;
}

BiNode* bi_shader_node_add_node(BiShaderNode *shader_node,BiNode* node)
{
  node->parent = (BiNodeBase*)shader_node;
  return array_add_object(&shader_node->children,node);
}

BiNode* bi_shader_node_remove_node(BiShaderNode *shader_node,BiNode* node)
{
  if( node && node == array_remove_object(&shader_node->children,node) ){
    node->parent = NULL;
    return node;
  }
  return NULL;
}

// Timer
BiTimer* bi_shader_node_add_timer(BiShaderNode* shader_node,BiTimer* timer){
  return bi_timers_add( &shader_node->timers,timer);
}
BiTimer* bi_shader_node_remove_timer(BiShaderNode* shader_node,BiTimer* timer){
  return bi_timers_remove( &shader_node->timers,timer);
}
