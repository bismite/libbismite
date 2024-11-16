#ifndef __BI_FRAMEBUFFER_NODE_H__
#define __BI_FRAMEBUFFER_NODE_H__

#include <stdbool.h>
#include <bi/bi_gl.h>
#include <bi/node_base.h>
#include <bi/framebuffer.h>

#define BI_LAYER_MAX_TEXTURES 16

typedef struct _BiNode BiNode;
typedef struct _BiShaderNode BiShaderNode;
typedef struct _BiFramebufferNode BiFramebufferNode;

struct _BiFramebufferNode {
  BI_NODE_HEADER;
  int w;
  int h;
  BiBlendFactor blend_factor;
  BiFramebuffer framebuffer;
};

extern BiFramebufferNode* bi_framebuffer_node_init(BiFramebufferNode* framebuffer_node);
extern BiFramebufferNode* bi_framebuffer_node_init_with_size(BiFramebufferNode* framebuffer_node,int w, int h);
extern int bi_framebuffer_node_get_z_order(BiFramebufferNode* framebuffer_node);
extern void bi_framebuffer_node_set_z_order(BiFramebufferNode* framebuffer_node,int z);
// Tree
extern BiShaderNode* bi_framebuffer_node_add_shader_node(BiFramebufferNode* framebuffer_node, BiShaderNode* obj);
extern BiShaderNode* bi_framebuffer_node_remove_shader_node(BiFramebufferNode* framebuffer_node, BiShaderNode* obj);
extern BiFramebufferNode* bi_framebuffer_node_add_framebuffer_node(BiFramebufferNode* framebuffer_node, BiFramebufferNode* obj);
extern BiFramebufferNode* bi_framebuffer_node_remove_framebuffer_node(BiFramebufferNode* framebuffer_node, BiFramebufferNode* obj);
// Timer
static inline BiTimer* bi_framebuffer_node_add_timer(BiFramebufferNode* lg,BiTimer* timer){ return bi_timers_add(&lg->timers,timer); }
static inline BiTimer* bi_framebuffer_node_remove_timer(BiFramebufferNode* lg,BiTimer* timer){ return bi_timers_remove(&lg->timers,timer); }
// Render
extern void bi_framebuffer_node_clear(BiFramebufferNode* framebuffer_node,uint8_t r,uint8_t g,uint8_t b,uint8_t a);
extern void bi_framebuffer_node_draw(BiFramebufferNode* framebuffer_node,BiContext* context);

#endif // __BI_FRAMEBUFFER_NODE_H__