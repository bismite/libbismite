#ifndef __BI_CORE_LAYER_H__
#define __BI_CORE_LAYER_H__

#include <bi/bi_gl.h>
#include <stdbool.h>
#include <bi/array.h>
#include <bi/framebuffer.h>

#define BI_LAYER_MAX_TEXTURES 16

typedef struct _BiNode BiNode;
typedef struct _BiTexture BiTexture;
typedef struct _BiShader BiShader;
typedef struct _BiLayerHeader BiLayerHeader;
typedef struct _BiLayerBlendFactor BiLayerBlendFactor;
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

struct _BiLayerBlendFactor {
  GLenum src;
  GLenum dst;
  GLenum alpha_src;
  GLenum alpha_dst;
};

struct _BiLayer {
  BiLayerHeader header;
  double time_scale;
  BiLayerBlendFactor blend_factor;
  GLfloat camera_x;
  GLfloat camera_y;
  bool projection_centering;
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
  BiLayerHeader header;
  double time_scale;
  BiLayerBlendFactor blend_factor;
  Array layers;
  BiFramebuffer framebuffer;
  bool interaction_enabled;
};

extern void bi_layer_init(BiLayer* layer);
extern void bi_layer_group_init(BiLayerGroup* layer_group);

//
// layer
//
static inline int bi_layer_get_z_order(BiLayer* layer){ return layer->header.z_order; }
static inline void bi_layer_set_z_order(BiLayer* layer,int z){ layer->header.z_order = z; }

//
// layer group
//
static inline int bi_layer_group_get_z_order(BiLayerGroup* layer_group){ return layer_group->header.z_order; }
static inline void bi_layer_group_set_z_order(BiLayerGroup* layer_group,int z){ layer_group->header.z_order = z; }

extern void bi_layer_group_add_layer(BiLayerGroup* layer_group, BiLayer* obj);
extern void bi_layer_group_remove_layer(BiLayerGroup* layer_group, BiLayer* obj);
extern void bi_layer_group_add_layer_group(BiLayerGroup* layer_group, BiLayerGroup* obj);
extern void bi_layer_group_remove_layer_group(BiLayerGroup* layer_group, BiLayerGroup* obj);
extern void bi_layer_group_update_order(BiLayerGroup* layer_group);

//
static inline void bi_set_blend_factor(BiLayerBlendFactor* b, GLenum src, GLenum dst, GLenum alpha_src, GLenum alpha_dst ) {
  b->src = src;
  b->dst = dst;
  b->alpha_src = alpha_src;
  b->alpha_dst = alpha_dst;
}

#endif
