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

extern BiFramebuffer* bi_framebuffer_init(BiFramebuffer *fb,int w,int h);
extern void bi_framebuffer_save_png_image(BiFramebuffer *fb,const char *filename);
static inline void bi_framebuffer_clear(BiFramebuffer *fb,uint8_t r,uint8_t g,uint8_t b,uint8_t a)
{
  glBindFramebuffer(GL_FRAMEBUFFER, fb->framebuffer_id);
  glClearColor(r/255.0, g/255.0, b/255.0, a/255.0);
  glClear(GL_COLOR_BUFFER_BIT);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

#endif
