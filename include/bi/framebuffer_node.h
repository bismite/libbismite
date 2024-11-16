#ifndef __BI_FRAMEBUFFER_NODE_H__
#define __BI_FRAMEBUFFER_NODE_H__

#include <stdint.h>

typedef struct _BiNode BiNode;
typedef struct _BiContext BiContext;

// Render
extern void bi_framebuffer_node_clear(BiNode*,uint8_t r,uint8_t g,uint8_t b,uint8_t a);
extern void bi_framebuffer_node_draw(BiNode*,BiContext*);

#endif // __BI_FRAMEBUFFER_NODE_H__
