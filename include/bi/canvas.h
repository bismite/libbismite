#ifndef __BISMITE_CANVAS_H__
#define __BISMITE_CANVAS_H__

#include <bi/bi_gl.h>
#include <stdbool.h>
#include <bi/framebuffer.h>
#include <bi/blend_factor.h>
#include <bi/layer.h>

typedef struct _BiNode BiNode;
typedef struct _BiTexture BiTexture;
typedef struct _BiShader BiShader;

typedef struct _BiCanvas {
  int w;
  int h;
  BiFramebuffer framebuffer;
  BiLayerBlendFactor blend_factor;
  BiShader *shader;
  GLfloat shader_attributes[4];
  BiTexture* textures[BI_LAYER_MAX_TEXTURES];
} BiCanvas;

extern BiCanvas* bi_canvas_init(BiCanvas* canvas,int w,int h);
extern BiCanvas* bi_canvas_init_with_framebuffer(BiCanvas* canvas,BiFramebuffer *src);
extern void bi_canvas_draw(BiCanvas* canvas,BiNode* node);
extern void bi_canvas_clear(BiCanvas* canvas,uint8_t r,uint8_t g,uint8_t b,uint8_t a);
extern BiTexture* bi_canvas_to_texture(BiCanvas* canvas,BiTexture* texture);
extern void bi_canvas_save_png_image(BiCanvas* canvas,const char *filename);

#endif
