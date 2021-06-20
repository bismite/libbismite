#ifndef __BI_CORE_LAYER_H__
#define __BI_CORE_LAYER_H__

#include <bi/bi_gl.h>
#include <stdbool.h>
#include <bi/array.h>

typedef struct _BiNode BiNode;
typedef struct _BiTexture BiTexture;
typedef struct _BiShader BiShader;
typedef struct _BiLayerHeader BiLayerHeader;
typedef struct _BiLayer BiLayer;
typedef struct _BiLayerGroup BiLayerGroup;
typedef struct _BiPostProcess BiPostProcess;

typedef enum {
  BI_LAYER_TYPE_LAYER,
  BI_LAYER_TYPE_LAYER_GROUP
} BiLayerType;

struct _BiLayerHeader {
  BiLayerType type;
  int z_order;
  int index;
};

struct _BiLayer {
  BiLayerHeader header;
  GLfloat camera_x;
  GLfloat camera_y;
  bool projection_centering;
  BiNode* root;
  GLenum blend_src;
  GLenum blend_dst;
  BiTexture* textures[8];
  BiShader *shader;
  GLfloat optional_shader_attributes[4];
};

struct _BiLayerGroup {
  BiLayerHeader header;
  Array layers;
  Array post_processes;
};

struct _BiPostProcess {
  BiShader* shader;
  GLuint framebuffer;
  GLuint texture;
  GLfloat optional_shader_attributes[4];
};

extern void bi_layer_init(BiLayer* layer);
extern void bi_layer_group_init(BiLayerGroup* layer_group);
extern void bi_post_process_init(BiPostProcess* post_process);

// layer
static inline void bi_layer_set_z_order(BiLayer* layer,int z){ layer->header.z_order = z; }

// layer group
extern void bi_layer_group_add_layer(BiLayerGroup* layer_group, BiLayer* obj);
extern void bi_layer_group_remove_layer(BiLayerGroup* layer_group, BiLayer* obj);
extern void bi_layer_group_add_layer_group(BiLayerGroup* layer_group, BiLayerGroup* obj);
extern void bi_layer_group_remove_layer_group(BiLayerGroup* layer_group, BiLayerGroup* obj);
extern void bi_layer_group_update_order(BiLayerGroup* layer_group);

extern void bi_layer_group_add_post_process(BiLayerGroup* layer_group, BiPostProcess *obj);
extern void bi_layer_group_remove_post_process(BiLayerGroup* layer_group, BiPostProcess *obj);

#endif
