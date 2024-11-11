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

BiLayerGroup* bi_layer_group_init_with_size(BiLayerGroup* layer_group,int w, int h)
{
  bi_node_base_init((BiNodeBase*)layer_group,BI_LAYER_GROUP);
  layer_group->blend_factor = BI_BLEND_FACTOR_DEFAULT;
  bi_framebuffer_init(&layer_group->framebuffer,w,h);
  layer_group->w = w;
  layer_group->h = h;
  return layer_group;
}

BiLayerGroup* bi_layer_group_init(BiLayerGroup* layer_group)
{
  GLint dims[4] = {0};
  glGetIntegerv(GL_VIEWPORT, dims);
  return bi_layer_group_init_with_size(layer_group,dims[2],dims[3]);
}

int bi_layer_group_get_z_order(BiLayerGroup* layer_group)
{
  return layer_group->z;
}

void bi_layer_group_set_z_order(BiLayerGroup* layer_group,int z)
{
  layer_group->z = z;
}

BiLayer* bi_layer_group_add_layer(BiLayerGroup* layer_group, BiLayer* obj)
{
  obj->parent = (BiNodeBase*)layer_group;
  return array_add_object(&layer_group->children,obj);
}

BiLayer* bi_layer_group_remove_layer(BiLayerGroup* layer_group, BiLayer* obj)
{
  if( obj && obj == array_remove_object(&layer_group->children,obj) ){
    obj->parent = NULL;
    return obj;
  }
  return NULL;
}

BiLayerGroup* bi_layer_group_add_layer_group(BiLayerGroup* layer_group, BiLayerGroup* obj)
{
  obj->parent = (BiNodeBase*)layer_group;
  return array_add_object(&layer_group->children,obj);
}

BiLayerGroup* bi_layer_group_remove_layer_group(BiLayerGroup* layer_group, BiLayerGroup* obj)
{
  if( obj && obj == array_remove_object(&layer_group->children,obj) ){
    obj->parent = NULL;
    return obj;
  }
  return NULL;
}

//
// LayerGroup Draw
//
void bi_layer_group_clear(BiLayerGroup* canvas,uint8_t r,uint8_t g,uint8_t b,uint8_t a)
{
  glBindFramebuffer(GL_FRAMEBUFFER, canvas->framebuffer.framebuffer_id);
  glClearColor(r/255.0, g/255.0, b/255.0, a/255.0);
  glClear(GL_COLOR_BUFFER_BIT);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void bi_layer_group_draw(BiLayerGroup* canvas, BiContext* context)
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
  bi_render_layer_group( context, (BiNodeBase*)canvas, NULL, rendering_context );
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
  if(layer && layer->parent) return bi_layer_group_remove_layer((BiLayerGroup*)layer->parent,layer);
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
