#include <bi/layer.h>
#include <bi/node.h>

void bi_layer_init(BiLayer* layer)
{
  layer->header.type = BI_LAYER_TYPE_LAYER;
  layer->header.z_order = 0;
  layer->root = NULL;
  layer->camera_x = 0;
  layer->camera_y = 0;
  layer->projection_centering = false;
  layer->blend_src = GL_SRC_ALPHA;
  layer->blend_dst = GL_ONE_MINUS_SRC_ALPHA;
  for(int i=0;i<8;i++) {
    layer->textures[i] = NULL;
  }
  layer->shader = NULL;
  for(int i=0;i<4;i++) {
    layer->optional_shader_attributes[i] = 0;
  }
}

void bi_layer_group_init(BiLayerGroup* layer_group)
{
  layer_group->header.type = BI_LAYER_TYPE_LAYER_GROUP;
  layer_group->header.z_order = 0;
  array_init(&layer_group->layers);
  array_init(&layer_group->post_processes);
}

void bi_post_process_init(BiPostProcess* post_process)
{
  // shader
  post_process->shader = NULL;
  // frame buffer
  glGenFramebuffers(1, &post_process->framebuffer);
  glBindFramebuffer(GL_FRAMEBUFFER, post_process->framebuffer);
  glGenTextures(1, &post_process->texture);
  glBindTexture(GL_TEXTURE_2D, post_process->texture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  GLint dims[4] = {0};
  glGetIntegerv(GL_VIEWPORT, dims);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, dims[2], dims[3], 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, post_process->texture, 0);
  // unbind
  glBindTexture(GL_TEXTURE_2D, 0);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

//
// Layer Group
//
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

void bi_layer_group_add_post_process(BiLayerGroup* layer_group, BiPostProcess *obj)
{
  array_add_object(&layer_group->post_processes,obj);
}

void bi_layer_group_remove_post_process(BiLayerGroup* layer_group, BiPostProcess *obj)
{
  array_remove_object(&layer_group->post_processes,obj);
}
