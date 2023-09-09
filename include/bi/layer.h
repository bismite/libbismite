#ifndef __BISMITE_LAYER_H__
#define __BISMITE_LAYER_H__
#include <stdbool.h>
#include <bi/bi_gl.h>
#include <bi/node_base.h>
#include <bi/timer.h>
#include <bi/array.h>
#include <bi/framebuffer.h>
#include <bi/blend_factor.h>

#define BI_LAYER_MAX_TEXTURES 16

typedef struct _BiNode BiNode;
typedef struct _BiTexture BiTexture;
typedef struct _BiShader BiShader;

typedef struct _BiLayer BiLayer;
typedef struct _BiLayerGroup BiLayerGroup;

#define BI_LAYER_HEADER void* _render_function_; \
                        BiLayerBlendFactor blend_factor;

typedef struct _BiLayerBase {
  BI_NODE_HEADER;
  BI_LAYER_HEADER;
} BiLayerBase;

struct _BiLayer {
  BI_NODE_HEADER;
  BI_LAYER_HEADER;
  GLfloat camera_x;
  GLfloat camera_y;
  BiNode* root;
  BiTexture* textures[BI_LAYER_MAX_TEXTURES];
  BiShader *shader;
  GLfloat shader_extra_data[16];
};

struct _BiLayerGroup {
  BI_NODE_HEADER;
  BI_LAYER_HEADER;
  Array layers;
  BiFramebuffer framebuffer;
};

//
// layer group
//
extern BiLayerGroup* bi_layer_group_init(BiLayerGroup* layer_group);
extern int bi_layer_group_get_z_order(BiLayerGroup* layer_group);
extern void bi_layer_group_set_z_order(BiLayerGroup* layer_group,int z);
// Tree
extern void bi_layer_group_add_layer(BiLayerGroup* layer_group, BiLayer* obj);
extern void bi_layer_group_remove_layer(BiLayerGroup* layer_group, BiLayer* obj);
extern void bi_layer_group_add_layer_group(BiLayerGroup* layer_group, BiLayerGroup* obj);
extern void bi_layer_group_remove_layer_group(BiLayerGroup* layer_group, BiLayerGroup* obj);
extern void bi_layer_group_update_order(BiLayerGroup* layer_group);
// Timer
static inline BiTimer* bi_layer_group_add_timer(BiLayerGroup* lg,BiTimer* timer){ return bi_timers_add(&lg->timers,timer); }
static inline BiTimer* bi_layer_group_remove_timer(BiLayerGroup* lg,BiTimer* timer){ return bi_timers_remove(&lg->timers,timer); }

//
// layer
//
extern BiLayer* bi_layer_init(BiLayer* layer);
extern BiLayer* bi_layer_init_as_postprocess(BiLayer* layer);
static inline void bi_layer_remove_from_parent(BiLayer* layer){ bi_layer_group_remove_layer( (BiLayerGroup*)layer->parent, layer); }
static inline int bi_layer_get_z_order(BiLayer* layer){ return layer->z; }
static inline void bi_layer_set_z_order(BiLayer* layer,int z){ layer->z = z; }
// Timer
static inline BiTimer* bi_layer_add_timer(BiLayer* layer,BiTimer* timer){ return bi_timers_add(&layer->timers,timer); }
static inline BiTimer* bi_layer_remove_timer(BiLayer* layer,BiTimer* timer){ return bi_timers_remove(&layer->timers,timer); }

#endif
