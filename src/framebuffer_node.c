#include <bi/shader_node.h>
#include <bi/node.h>
#include <bi/render.h>
#include <bi/context.h>

BiFramebufferNode* bi_framebuffer_node_init_with_framebuffer(BiFramebufferNode* fbnode,int w, int h,BiFramebuffer fb)
{
  bi_node_base_init((BiNodeBase*)fbnode,BI_FRAMEBUFFER_NODE);
  fbnode->blend_factor = BI_BLEND_FACTOR_DEFAULT;
  fbnode->w = w;
  fbnode->h = h;
  fbnode->framebuffer = fb;
  fbnode->color = RGBA(0,0,0,0);
  return fbnode;
}

BiFramebufferNode* bi_framebuffer_node_init(BiFramebufferNode* fbnode,int w, int h)
{
  BiFramebuffer fb;
  bi_framebuffer_init(&fb,w,h);
  return bi_framebuffer_node_init_with_framebuffer(fbnode,w,h,fb);
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
