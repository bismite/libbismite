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
  bool flip_horizontal;
  bool flip_vertical;
};


extern void bi_texture_init(BiTexture* texture);
extern bool bi_texture_load_from_memory(BiTexture* texture, void* buffer, size_t size, bool antialiase);
extern bool bi_texture_load_from_file(BiTexture* texture, const char* filename, bool antialiase);

extern void bi_texture_mapping_init(BiTextureMapping* texture_mapping);
extern void bi_texture_mapping_set_bound(BiTextureMapping* texture_mapping, uint16_t x, uint16_t y, uint16_t w, uint16_t h);

#endif
