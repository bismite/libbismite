#include <bi/layer.h>
#include <bi/node.h>

void bi_layer_init(BiLayer* layer)
{
  layer->header.type = BI_LAYER_TYPE_LAYER;
  layer->header.z_order = 0;

  layer->time_scale = 1.0;

  layer->blend_factor.src = GL_SRC_ALPHA;
  layer->blend_factor.dst = GL_ONE_MINUS_SRC_ALPHA;
  layer->blend_factor.alpha_src = GL_ONE;
  layer->blend_factor.alpha_dst = GL_ONE_MINUS_SRC_ALPHA;

  layer->root = NULL;
  layer->camera_x = 0;
  layer->camera_y = 0;
  layer->projection_centering = false;
  for(int i=0;i<BI_LAYER_MAX_TEXTURES;i++) {
    layer->textures[i] = NULL;
  }
  layer->shader = NULL;
  for(int i=0;i<4;i++) {
    layer->shader_attributes[i] = 0;
  }
  // Post Process
  layer->post_process.shader = NULL;
  layer->post_process.framebuffer_enabled = false;
  for(int i=0;i<4;i++) {
    layer->post_process.shader_attributes[i] = 0;
  }
  layer->post_process.blend_factor.src = GL_ONE;
  layer->post_process.blend_factor.dst = GL_ONE_MINUS_SRC_ALPHA;
  layer->post_process.blend_factor.alpha_src = GL_ONE;
  layer->post_process.blend_factor.alpha_dst = GL_ONE_MINUS_SRC_ALPHA;
}

//
// Layer Group
//
void bi_layer_group_init(BiLayerGroup* layer_group)
{
  layer_group->header.type = BI_LAYER_TYPE_LAYER_GROUP;
  layer_group->header.z_order = 0;

  layer_group->time_scale = 1.0;

  layer_group->blend_factor.src = GL_ONE;
  layer_group->blend_factor.dst = GL_ONE_MINUS_SRC_ALPHA;
  layer_group->blend_factor.alpha_src = GL_ONE;
  layer_group->blend_factor.alpha_dst = GL_ONE_MINUS_SRC_ALPHA;

  array_init(&layer_group->layers);
  bi_framebuffer_init(&layer_group->framebuffer);
  layer_group->interaction_enabled = true;
}

static int layer_order_compare(const void *_a, const void *_b )
{
  const BiLayerHeader *a = *(BiLayerHeader**)_a;
  const BiLayerHeader *b = *(BiLayerHeader**)_b;
  return a->z_order == b->z_order ? a->index - b->index : a->z_order - b->z_order;
}

void bi_layer_group_update_order(BiLayerGroup* layer_group)
{
  int size = layer_group->layers.size;
  BiLayerHeader** objects = (BiLayerHeader**)layer_group->layers.objects;
  for( int i=0; i<size; i++ ) { objects[i]->index = i; }
  qsort( objects, size, sizeof(void*), layer_order_compare );
  for( int i=0; i<size; i++ ) { objects[i]->index = i; }
}

void bi_layer_group_add_layer(BiLayerGroup* layer_group, BiLayer* obj)
{
  array_add_object(&layer_group->layers,obj);
  bi_layer_group_update_order(layer_group);
}

void bi_layer_group_remove_layer(BiLayerGroup* layer_group, BiLayer* obj)
{
  array_remove_object(&layer_group->layers,obj);
}

void bi_layer_group_add_layer_group(BiLayerGroup* layer_group, BiLayerGroup* obj)
{
  array_add_object(&layer_group->layers,obj);
  bi_layer_group_update_order(layer_group);
}

void bi_layer_group_remove_layer_group(BiLayerGroup* layer_group, BiLayerGroup* obj)
{
  array_remove_object(&layer_group->layers,obj);
}
