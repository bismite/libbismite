#include <bi/layer.h>
#include <bi/node.h>
#include <bi/render.h>

extern void render_postprocess(BiContext* context,
                               BiNodeBase *layer,
                               BiFramebuffer* fb,
                               BiRenderingContext rc
                             );
extern void render_layer(BiContext* context,
                         BiNodeBase *layer,
                         BiFramebuffer *fb,
                         BiRenderingContext rc
                       );

//
// Layer Group
//

BiLayerGroup* bi_layer_group_init(BiLayerGroup* layer_group)
{
  bi_node_base_init((BiNodeBase*)layer_group,BI_LAYER_GROUP);
  layer_group->blend_factor = BI_BLEND_FACTOR_DEFAULT;
  GLint dims[4] = {0};
  glGetIntegerv(GL_VIEWPORT, dims);
  bi_framebuffer_init(&layer_group->framebuffer,dims[2],dims[3]);
  //
  return layer_group;
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
