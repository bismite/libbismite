#ifndef __BI_CORE_TEXTURE_H__
#define __BI_CORE_TEXTURE_H__

#include <GL/glew.h>
#include <bi/bi_sdl.h>
#include <stdint.h>
#include <stdbool.h>

struct _BiTextureImage;
typedef struct _BiTextureImage BiTextureImage;

struct _BiTexture;
typedef struct _BiTexture BiTexture;

struct _BiTextureImage {
  GLuint texture_id;
  uint16_t w;
  uint16_t h;
  GLuint _texture_unit;
};

struct _BiTexture {
  BiTextureImage *texture_image;
  uint16_t x,y,w,h;
  double boundary[4]; // left,top,right,bottom
};

extern bool bi_load_texture(const char* filename, BiTextureImage* texture_image, bool antialiase);
extern void bi_set_texture_boundary(BiTexture* texture, uint16_t x, uint16_t y, uint16_t w, uint16_t h);

#endif
