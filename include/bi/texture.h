#ifndef __BISMITE_TEXTURE_H__
#define __BISMITE_TEXTURE_H__

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <bi/bi_gl.h>

typedef struct _BiFramebuffer BiFramebuffer;

typedef struct _BiTexture {
  GLuint texture_id;
  uint16_t w;
  uint16_t h;
  GLuint _texture_unit;
} BiTexture;

// Texture
extern BiTexture* bi_texture_init_with_file(BiTexture*, void* buffer, size_t size,bool straight_alpha);
extern BiTexture* bi_texture_init_with_filename(BiTexture*, const char* filename,bool straight_alpha);
extern BiTexture* bi_texture_init_with_framebuffer(BiTexture*, BiFramebuffer*);
extern BiTexture* bi_texture_init_with_texture_id(BiTexture*, int w, int h, GLuint texture_id);
extern void bi_texture_set_anti_alias(BiTexture* texture,bool anti_alias);
extern void bi_texture_delete(BiTexture* texture);

#endif
