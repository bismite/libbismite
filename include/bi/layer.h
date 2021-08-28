#ifndef __BI_CORE_LAYER_H__
#define __BI_CORE_LAYER_H__

#include <bi/bi_gl.h>
#include <stdbool.h>
#include <bi/array.h>

#define BI_LAYER_MAX_TEXTURES 16

typedef struct _BiNode BiNode;
typedef struct _BiTexture BiTexture;
typedef struct _BiShader BiShader;
typedef struct _BiLayerHeader BiLayerHeader;
typedef struct _BiLayer BiLayer;
typedef struct _BiLayerGroup BiLayerGroup;
typedef struct _BiPostProcess BiPostProcess;
typedef struct _BiFramebuffer BiFramebuffer;

typedef enum {
  BI_LAYER_TYPE_LAYER,
  BI_LAYER_TYPE_LAYER_GROUP
} BiLayerType;

struct _BiLayerHeader {
  BiLayerType type;
  int z_order;
  int index;
};

struct _BiFramebuffer {
  GLuint framebuffer_id;
  GLuint texture_id;
  int w;
  int h;
};

struct _BiLayer {
  BiLayerHeader header;
  GLfloat camera_x;
  GLfloat camera_y;
  bool projection_centering;
  BiNode* root;
  GLenum blend_src;
  GLenum blend_dst;
  GLenum blend_alpha_src;
  GLenum blend_alpha_dst;
  BiTexture* textures[BI_LAYER_MAX_TEXTURES];
  BiShader *shader;
  GLfloat optional_shader_attributes[4];
  BiPostProcess *post_process;
  BiFramebuffer _framebuffer;
  bool _framebuffer_enabled;
};

struct _BiLayerGroup {
  BiLayerHeader header;
  Array layers;
  BiFramebuffer framebuffer;
};

struct _BiPostProcess {
  BiShader* shader;
  BiFramebuffer framebuffer;
  GLfloat attributes[4];
};

extern void bi_layer_init(BiLayer* layer);
extern void bi_layer_group_init(BiLayerGroup* layer_group);
extern void bi_post_process_init(BiPostProcess* post_process,BiShader* shader);

//
// layer
//
void bi_layer_set_framebuffer_enabled(BiLayer* layer,bool framebuffer_enabled);
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

#endif
