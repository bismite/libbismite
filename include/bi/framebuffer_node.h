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
  BiColor color;
};

extern BiFramebufferNode* bi_framebuffer_node_init(BiFramebufferNode*,int w,int h);
extern BiFramebufferNode* bi_framebuffer_node_init_with_framebuffer(BiFramebufferNode*,int w,int h,BiFramebuffer);
extern int bi_framebuffer_node_get_z_order(BiFramebufferNode*);
extern void bi_framebuffer_node_set_z_order(BiFramebufferNode*,int);
// Tree
extern BiShaderNode* bi_framebuffer_node_add_shader_node(BiFramebufferNode*, BiShaderNode*);
extern BiShaderNode* bi_framebuffer_node_remove_shader_node(BiFramebufferNode*, BiShaderNode*);
extern BiFramebufferNode* bi_framebuffer_node_add_framebuffer_node(BiFramebufferNode*, BiFramebufferNode*);
extern BiFramebufferNode* bi_framebuffer_node_remove_framebuffer_node(BiFramebufferNode*, BiFramebufferNode*);
// Timer
static inline BiTimer* bi_framebuffer_node_add_timer(BiFramebufferNode* f,BiTimer* t){ return bi_timers_add(&f->timers,t); }
static inline BiTimer* bi_framebuffer_node_remove_timer(BiFramebufferNode* f,BiTimer* t){ return bi_timers_remove(&f->timers,t); }
// Render
extern void bi_framebuffer_node_clear(BiFramebufferNode*,uint8_t r,uint8_t g,uint8_t b,uint8_t a);
extern void bi_framebuffer_node_draw(BiFramebufferNode*,BiContext*);

#endif // __BI_FRAMEBUFFER_NODE_H__
