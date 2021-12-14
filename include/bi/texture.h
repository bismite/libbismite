#ifndef __BI_CORE_TEXTURE_H__
#define __BI_CORE_TEXTURE_H__

#include <stdint.h>
#include <stdbool.h>
#include <bi/bi_gl.h>
#include <bi/bi_sdl.h>

typedef struct _BiTexture BiTexture;
typedef struct _BiTextureMapping BiTextureMapping;
typedef struct _BiLayerGroup BiLayerGroup;

struct _BiTexture {
  GLuint texture_id;
  uint16_t w;
  uint16_t h;
  GLuint _texture_unit;
};

struct _BiTextureMapping {
  BiTexture *texture;
  uint16_t x,y,w,h;
  bool flip_horizontal;
  bool flip_vertical;
  GLfloat _uv[4]; // left,top,right,bottom
};

extern BiTexture* bi_texture_init_with_pixels(BiTexture* texture, int w, int h, void* pixels, GLint internal_format, GLenum format, bool antialias);
extern BiTexture* bi_texture_init_with_file(BiTexture* texture, void* buffer, size_t size, bool antialiase);
extern BiTexture* bi_texture_init_with_filename(BiTexture* texture, const char* filename, bool antialiase);

extern void bi_texture_delete(BiTexture* texture);

extern BiTextureMapping* bi_texture_mapping_init(BiTextureMapping* texture_mapping,BiTexture* texture);
extern void bi_texture_mapping_set_bound(BiTextureMapping* texture_mapping, uint16_t x, uint16_t y, uint16_t w, uint16_t h);

#endif
