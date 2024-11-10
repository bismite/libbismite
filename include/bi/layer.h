#ifndef __BISMITE_LAYER_H__
#define __BISMITE_LAYER_H__
#include <stdbool.h>
#include <bi/bi_gl.h>
#include <bi/node_base.h>
#include <bi/array.h>
#include <bi/framebuffer.h>

#define BI_LAYER_MAX_TEXTURES 16

typedef struct _BiTexture BiTexture;
typedef struct _BiNode BiNode;
typedef struct _BiShader BiShader;
typedef struct _BiLayer BiLayer;
typedef struct _BiLayerGroup BiLayerGroup;

struct _BiLayer {
  BI_NODE_HEADER;
  void* _render_function_;
  BiBlendFactor blend_factor;
  GLfloat camera_x;
  GLfloat camera_y;
  BiTexture* textures[BI_LAYER_MAX_TEXTURES];
  BiShader *shader;
  GLfloat shader_extra_data[16];
};

struct _BiLayerGroup {
  BI_NODE_HEADER;
  int w;
  int h;
  BiBlendFactor blend_factor;
  BiFramebuffer framebuffer;
};

//
// layer group
//
extern BiLayerGroup* bi_layer_group_init(BiLayerGroup* layer_group);
extern BiLayerGroup* bi_layer_group_init_with_size(BiLayerGroup* layer_group,int w, int h);
extern int bi_layer_group_get_z_order(BiLayerGroup* layer_group);
extern void bi_layer_group_set_z_order(BiLayerGroup* layer_group,int z);
// Tree
extern BiLayer* bi_layer_group_add_layer(BiLayerGroup* layer_group, BiLayer* obj);
extern BiLayer* bi_layer_group_remove_layer(BiLayerGroup* layer_group, BiLayer* obj);
extern BiLayerGroup* bi_layer_group_add_layer_group(BiLayerGroup* layer_group, BiLayerGroup* obj);
extern BiLayerGroup* bi_layer_group_remove_layer_group(BiLayerGroup* layer_group, BiLayerGroup* obj);
// Timer
static inline BiTimer* bi_layer_group_add_timer(BiLayerGroup* lg,BiTimer* timer){ return bi_timers_add(&lg->timers,timer); }
static inline BiTimer* bi_layer_group_remove_timer(BiLayerGroup* lg,BiTimer* timer){ return bi_timers_remove(&lg->timers,timer); }
// Render
extern void bi_layer_group_clear(BiLayerGroup* layer_group,uint8_t r,uint8_t g,uint8_t b,uint8_t a);
extern void bi_layer_group_draw(BiLayerGroup* layer_group,BiContext* context);

//
// layer
//
extern BiLayer* bi_layer_init(BiLayer* layer);
extern BiLayer* bi_layer_init_as_postprocess(BiLayer* layer);
static inline int bi_layer_get_z_order(BiLayer* layer){ return layer->z; }
static inline void bi_layer_set_z_order(BiLayer* layer,int z){ layer->z = z; }
// Tree
extern BiLayer* bi_layer_remove_from_parent(BiLayer* layer);
extern BiNode* bi_layer_add_node(BiLayer *layer,BiNode* node);
extern BiNode* bi_layer_remove_node(BiLayer *layer,BiNode* node);
// Timer
extern BiTimer* bi_layer_add_timer(BiLayer* layer,BiTimer* timer);
extern BiTimer* bi_layer_remove_timer(BiLayer* layer,BiTimer* timer);

#endif
