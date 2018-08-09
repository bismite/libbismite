#ifndef __BI_CORE_RENDER_H__
#define __BI_CORE_RENDER_H__

struct _BiContext;
typedef struct _BiContext BiContext;

struct _BiLayer;
typedef struct _BiLayer BiLayer;

extern void bi_render_layer(BiContext* context,BiLayer* layer);

#endif
