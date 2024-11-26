#ifndef __BI_FRAMEBUFFER_H__
#define __BI_FRAMEBUFFER_H__

#include <bi/bi_gl.h>
#include <bi/texture.h>

// see GL_MAX_COLOR_ATTACHMENTS and GL_MAX_DRAW_BUFFERS
#define BI_FRAMEBUFFER_TEXTURE_MAX 8

typedef struct _BiFramebuffer BiFramebuffer;
struct _BiFramebuffer {
  GLuint framebuffer_id;
  BiTexture textures[BI_FRAMEBUFFER_TEXTURE_MAX];
  int texture_num;
  int w;
  int h;
  int viewport_w;
  int viewport_h;
};

extern BiFramebuffer* bi_framebuffer_init_with_texture_num(BiFramebuffer *fb,int w,int h,int texture_num);
static inline BiFramebuffer* bi_framebuffer_init(BiFramebuffer *fb,int w,int h) {
  return bi_framebuffer_init_with_texture_num(fb,w,h,1);
}
extern void bi_framebuffer_clear(BiFramebuffer *fb,uint8_t r,uint8_t g,uint8_t b,uint8_t a);

#endif
