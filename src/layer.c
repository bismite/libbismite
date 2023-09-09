#include <bi/layer.h>
#include <bi/node.h>
#include <bi/render.h>

extern void render_postprocess(BiContext* context,
                               BiLayerBase *layer_group,
                               BiFramebuffer* fb,
                               BiRenderingContext rc
                             );
extern void render_layer_group(BiContext* context,
                               BiLayerBase *layer_group,
                               BiFramebuffer* fb,
                               BiRenderingContext rc
                             );
extern void render_layer(BiContext* context,
                         BiLayerBase *layer,
                         BiFramebuffer *fb,
                         BiRenderingContext rc
                       );

BiLayer* bi_layer_init_as_postprocess(BiLayer* layer)
{
  bi_layer_init(layer);
  layer->_render_function_ = render_postprocess;
  return layer;
}

BiLayer* bi_layer_init(BiLayer* layer)
{
  bi_node_base_init((BiNodeBase*)layer);
  bi_timers_init(&layer->timers);
  layer->_render_function_ = render_layer;
  layer->blend_factor.src = GL_ONE;
  layer->blend_factor.dst = GL_ONE_MINUS_SRC_ALPHA;
  layer->blend_factor.alpha_src = GL_ONE;
  layer->blend_factor.alpha_dst = GL_ONE_MINUS_SRC_ALPHA;

  layer->root = NULL;
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

//
// Layer Group
//
BiLayerGroup* bi_layer_group_init(BiLayerGroup* layer_group)
{
  bi_node_base_init((BiNodeBase*)layer_group);
  bi_timers_init(&layer_group->timers);
  layer_group->_render_function_ = render_layer_group;
  layer_group->blend_factor.src = GL_ONE;
  layer_group->blend_factor.dst = GL_ONE_MINUS_SRC_ALPHA;
  layer_group->blend_factor.alpha_src = GL_ONE;
  layer_group->blend_factor.alpha_dst = GL_ONE_MINUS_SRC_ALPHA;

  array_init(&layer_group->layers);

  GLint dims[4] = {0};
  glGetIntegerv(GL_VIEWPORT, dims);
  bi_framebuffer_init(&layer_group->framebuffer,dims[2],dims[3]);
  //
  return layer_group;
}

static int layer_order_compare(const void *_a, const void *_b )
{
  const BiNodeBase *a = *(BiNodeBase**)_a;
  const BiNodeBase *b = *(BiNodeBase**)_b;
  return a->z == b->z ? a->index - b->index : a->z - b->z;
}

void bi_layer_group_update_order(BiLayerGroup* layer_group)
{
  int size = layer_group->layers.size;
  BiNodeBase** objects = (BiNodeBase**)layer_group->layers.objects;
  for( int i=0; i<size; i++ ) { objects[i]->index = i; }
  qsort( objects, size, sizeof(void*), layer_order_compare );
  for( int i=0; i<size; i++ ) { objects[i]->index = i; }
}

int bi_layer_group_get_z_order(BiLayerGroup* layer_group)
{
  return layer_group->z;
}

void bi_layer_group_set_z_order(BiLayerGroup* layer_group,int z)
{
  layer_group->z = z;
}

void bi_layer_group_add_layer(BiLayerGroup* layer_group, BiLayer* obj)
{
  obj->parent = (BiNodeBase*)layer_group;
  array_add_object(&layer_group->layers,obj);
  bi_layer_group_update_order(layer_group);
}

void bi_layer_group_remove_layer(BiLayerGroup* layer_group, BiLayer* obj)
{
  obj->parent = NULL;
  array_remove_object(&layer_group->layers,obj);
}

void bi_layer_group_add_layer_group(BiLayerGroup* layer_group, BiLayerGroup* obj)
{
  obj->parent = (BiNodeBase*)layer_group;
  array_add_object(&layer_group->layers,obj);
  bi_layer_group_update_order(layer_group);
}

void bi_layer_group_remove_layer_group(BiLayerGroup* layer_group, BiLayerGroup* obj)
{
  obj->parent = NULL;
  array_remove_object(&layer_group->layers,obj);
}
