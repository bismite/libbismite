#ifndef __BISMITE_TEXTURE_H__
#define __BISMITE_TEXTURE_H__

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <bi/bi_gl.h>

typedef struct _BiTexture {
  GLuint texture_id;
  uint16_t w;
  uint16_t h;
  GLuint _texture_unit;
} BiTexture;

//             Image                     Texture
//    0/0 ------------------      ---------------------   ^
//     |         y         |      |                   |   |
//     |     -------- ^    |      |          ------   |   |
//     |  x  |      | |    |      |          |    |   |   |
//     |     |      | h    |      | offset_x |    |   | outer_h
//     |     |      | |    |      |          |    |   |   |
//     |     -------- v    |      |          ------   |   |
//     |     <---w-->      |      |         offset_y  |   |
//     ---------------------     0/0 ------------------   v
//                                <----- outer_w ---->

typedef struct _BiTextureMapping {
  BiTexture *texture;
  uint16_t x,y,w,h;
  bool clipped;
  uint16_t offset_x,offset_y;
  uint16_t outer_w,outer_h;
  bool flip_horizontal;
  bool flip_vertical;
  GLfloat _uv[4]; // left,top,right,bottom
} BiTextureMapping;

// Texture
extern BiTexture* bi_texture_init_with_file(BiTexture* texture, void* buffer, size_t size,bool straight_alpha);
extern BiTexture* bi_texture_init_with_filename(BiTexture* texture, const char* filename,bool straight_alpha);
extern void bi_texture_set_anti_alias(BiTexture* texture,bool anti_alias);
extern void bi_texture_delete(BiTexture* texture);

// Texture Mapping
extern BiTextureMapping* bi_texture_mapping_init(BiTextureMapping* texture_mapping,BiTexture* texture);
extern void bi_texture_mapping_set_bound(BiTextureMapping* texture_mapping, uint16_t x, uint16_t y, uint16_t w, uint16_t h);
extern void bi_texture_mapping_set_clip(BiTextureMapping* texture_mapping, uint16_t x, uint16_t y, uint16_t w, uint16_t h);
extern void bi_texture_mapping_unset_clip(BiTextureMapping* texture_mapping);

#endif
