#ifndef __BI_CORE_TEXTURE_H__
#define __BI_CORE_TEXTURE_H__

#include <stdint.h>
#include <stdbool.h>
#include <bi/bi_gl.h>
#include <bi/bi_sdl.h>

struct _BiTexture;
typedef struct _BiTexture BiTexture;

struct _BiTextureMapping;
typedef struct _BiTextureMapping BiTextureMapping;

struct _BiTexture {
  GLuint texture_id;
  uint16_t w;
  uint16_t h;
  GLuint _texture_unit;
};

struct _BiTextureMapping {
  BiTexture *texture;
  uint16_t x,y,w,h;
  double boundary[4]; // left,top,right,bottom
};

extern bool bi_create_texture(void* buffer, size_t size, BiTexture* texture, bool antialiase);
extern bool bi_load_texture(const char* filename, BiTexture* texture, bool antialiase);
extern void bi_set_texture_mapping(BiTextureMapping* texture_mapping, uint16_t x, uint16_t y, uint16_t w, uint16_t h);

#endif
