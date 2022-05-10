#include <bi/layer.h>
#include <bi/node.h>

BiLayer* bi_layer_init(BiLayer* layer)
{
  bi_raw_node_init((BiRawNode*)layer,BI_NODE_TYPE_LAYER);

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
  return layer;
}

//
// Layer Group
//
BiLayerGroup* bi_layer_group_init(BiLayerGroup* layer_group)
{
  bi_raw_node_init((BiRawNode*)layer_group,BI_NODE_TYPE_LAYER_GROUP);

  layer_group->blend_factor.src = GL_ONE;
  layer_group->blend_factor.dst = GL_ONE_MINUS_SRC_ALPHA;
  layer_group->blend_factor.alpha_src = GL_ONE;
  layer_group->blend_factor.alpha_dst = GL_ONE_MINUS_SRC_ALPHA;

  array_init(&layer_group->layers);

  GLint dims[4] = {0};
  glGetIntegerv(GL_VIEWPORT, dims);
  bi_framebuffer_init(&layer_group->framebuffer,dims[2],dims[3]);
  return layer_group;
}

static int layer_order_compare(const void *_a, const void *_b )
{
  const BiRawNode *a = *(BiRawNode**)_a;
  const BiRawNode *b = *(BiRawNode**)_b;
  return a->z == b->z ? a->index - b->index : a->z - b->z;
}

void bi_layer_group_update_order(BiLayerGroup* layer_group)
{
  int size = layer_group->layers.size;
  BiRawNode** objects = (BiRawNode**)layer_group->layers.objects;
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
