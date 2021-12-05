#ifndef __BISMITE_RENDER_H__
#define __BISMITE_RENDER_H__

typedef struct _BiContext BiContext;
typedef struct _BiNode BiNode;

extern void bi_render(BiContext* context);
extern void bi_render_update_matrix(BiNode* n);
extern void bi_render_node_sort(BiNode* node);

#endif
