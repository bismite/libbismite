#ifndef __BISMITE_BLEND_FACTOR_H__
#define __BISMITE_BLEND_FACTOR_H__

#include <bi/bi_gl.h>

typedef struct _BiLayerBlendFactor BiLayerBlendFactor;
struct _BiLayerBlendFactor {
  GLenum src;
  GLenum dst;
  GLenum alpha_src;
  GLenum alpha_dst;
};

//
static inline void bi_set_blend_factor(BiLayerBlendFactor* b, GLenum src, GLenum dst, GLenum alpha_src, GLenum alpha_dst ) {
  b->src = src;
  b->dst = dst;
  b->alpha_src = alpha_src;
  b->alpha_dst = alpha_dst;
}

#endif
