#ifndef __BI_CORE_FRAMEBUFFER_H__
#define __BI_CORE_FRAMEBUFFER_H__

#include <bi/bi_gl.h>

typedef struct _BiFramebuffer BiFramebuffer;

struct _BiFramebuffer {
  GLuint framebuffer_id;
  GLuint texture_id;
  int w;
  int h;
};

extern void bi_framebuffer_init(BiFramebuffer *fb);

#endif