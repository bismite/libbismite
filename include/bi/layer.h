#ifndef __BI_CORE_LAYER_H__
#define __BI_CORE_LAYER_H__

#include <bi/bi_gl.h>
#include <stdbool.h>
#include <bi/array.h>
#include <bi/framebuffer.h>
#include <bi/node_base.h>
#include <bi/blend_factor.h>

#define BI_LAYER_MAX_TEXTURES 16

typedef struct _BiNode BiNode;
typedef struct _BiTexture BiTexture;
typedef struct _BiShader BiShader;

typedef struct _BiLayer BiLayer;
typedef struct _BiLayerGroup BiLayerGroup;
typedef struct _BiPostProcess BiPostProcess;

struct _BiLayer {
  BI_NODE_HEADER;
  BiLayerBlendFactor blend_factor;
  GLfloat camera_x;
  GLfloat camera_y;
  BiNode* root;
  BiTexture* textures[BI_LAYER_MAX_TEXTURES];
  BiShader *shader;
  GLfloat shader_attributes[4];
  // Post Process
  struct {
    BiShader* shader;
    BiLayerBlendFactor blend_factor;
    GLfloat shader_attributes[4];
    bool framebuffer_enabled;
  } post_process;
};

struct _BiLayerGroup {
  BI_NODE_HEADER;
  BiLayerBlendFactor blend_factor;
  Array layers;
  BiFramebuffer framebuffer;
};

extern BiLayer* bi_layer_init(BiLayer* layer);
extern BiLayerGroup* bi_layer_group_init(BiLayerGroup* layer_group);

//
// layer
//
static inline int bi_layer_get_z_order(BiLayer* layer){ return layer->z; }
static inline void bi_layer_set_z_order(BiLayer* layer,int z){ layer->z = z; }
// Timer
static inline void bi_layer_add_timer(BiLayer* layer,BiTimer* timer){ bi_raw_node_add_timer((BiRawNode*)layer,timer); }
static inline void bi_layer_remove_timer(BiLayer* layer,BiTimer* timer){ bi_raw_node_remove_timer((BiRawNode*)layer,timer); }

//
// layer group
//
extern int bi_layer_group_get_z_order(BiLayerGroup* layer_group);
extern void bi_layer_group_set_z_order(BiLayerGroup* layer_group,int z);
// Tree
extern void bi_layer_group_add_layer(BiLayerGroup* layer_group, BiLayer* obj);
extern void bi_layer_group_remove_layer(BiLayerGroup* layer_group, BiLayer* obj);
extern void bi_layer_group_add_layer_group(BiLayerGroup* layer_group, BiLayerGroup* obj);
extern void bi_layer_group_remove_layer_group(BiLayerGroup* layer_group, BiLayerGroup* obj);
extern void bi_layer_group_update_order(BiLayerGroup* layer_group);
// Timer
static inline void bi_layer_group_add_timer(BiLayerGroup* lg,BiTimer* timer){ bi_raw_node_add_timer((BiRawNode*)lg,timer); }
static inline void bi_layer_group_remove_timer(BiLayerGroup* lg,BiTimer* timer){ bi_raw_node_remove_timer((BiRawNode*)lg,timer); }

#endif
