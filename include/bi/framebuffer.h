#ifndef __BI_FRAMEBUFFER_H__
#define __BI_FRAMEBUFFER_H__

#include <bi/bi_gl.h>

typedef struct _BiFramebuffer BiFramebuffer;

struct _BiFramebuffer {
  GLuint framebuffer_id;
  GLuint texture_ids[8]; // GL_MAX_COLOR_ATTACHMENTS, GL_MAX_DRAW_BUFFERS
  int texture_num;
  int w;
  int h;
};

extern BiFramebuffer* bi_framebuffer_init(BiFramebuffer *fb,int w,int h);
extern BiFramebuffer* bi_framebuffer_init_with_texture_num(BiFramebuffer *fb,int w,int h,int texture_num);
extern void bi_framebuffer_save_png_image(BiFramebuffer *fb,const char *filename);
extern void bi_framebuffer_clear(BiFramebuffer *fb,uint8_t r,uint8_t g,uint8_t b,uint8_t a);

#endif
