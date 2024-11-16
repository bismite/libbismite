#include <bi/layer.h>
#include <bi/node.h>
#include <bi/render.h>
#include <bi/context.h>

extern void render_postprocess(BiContext* context,
                               BiNodeBase *layer,
                               BiFramebuffer* dst,
                               BiRenderingContext rc
                             );
extern void render_layer(BiContext* context,
                         BiNodeBase *layer,
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

BiLayer* bi_framebuffer_node_add_layer(BiFramebufferNode* framebuffer_node, BiLayer* obj)
{
  obj->parent = (BiNodeBase*)framebuffer_node;
  return array_add_object(&framebuffer_node->children,obj);
}

BiLayer* bi_framebuffer_node_remove_layer(BiFramebufferNode* framebuffer_node, BiLayer* obj)
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

BiLayer* bi_layer_init(BiLayer* layer)
{
  bi_node_base_init( (BiNodeBase*)layer, BI_LAYER );
  layer->_render_function_ = render_layer;
  layer->blend_factor = BI_BLEND_FACTOR_DEFAULT;

  layer->camera_x = 0;
  layer->camera_y = 0;
  for(int i=0;i<BI_LAYER_MAX_TEXTURES;i++) {
    layer->textures[i] = NULL;
  }
  layer->shader = NULL;
  for(int i=0;i<16;i++) {
    layer->shader_extra_data[i] = 0;
  }
  //
  return layer;
}

BiLayer* bi_layer_init_as_postprocess(BiLayer* layer)
{
  bi_layer_init(layer);
  layer->_render_function_ = render_postprocess;
  return layer;
}

BiLayer* bi_layer_remove_from_parent(BiLayer* layer)
{
  if(layer && layer->parent) return bi_framebuffer_node_remove_layer((BiFramebufferNode*)layer->parent,layer);
  return NULL;
}

BiNode* bi_layer_add_node(BiLayer *layer,BiNode* node)
{
  node->parent = (BiNodeBase*)layer;
  return array_add_object(&layer->children,node);
}

BiNode* bi_layer_remove_node(BiLayer *layer,BiNode* node)
{
  if( node && node == array_remove_object(&layer->children,node) ){
    node->parent = NULL;
    return node;
  }
  return NULL;
}

// Timer
BiTimer* bi_layer_add_timer(BiLayer* layer,BiTimer* timer){
  return bi_timers_add( &layer->timers,timer);
}
BiTimer* bi_layer_remove_timer(BiLayer* layer,BiTimer* timer){
  return bi_timers_remove( &layer->timers,timer);
}
