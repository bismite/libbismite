#ifndef __BISMITE_EXT_FONT_H__
#define __BISMITE_EXT_FONT_H__

#include <stdint.h>
#include <stdbool.h>

struct _BiTexture;
typedef struct _BiTexture BiTexture;

typedef struct {
  uint32_t version;
  uint16_t font_size;
  int16_t height;
  int16_t ascent;
  int16_t descent;
  uint32_t glyph_count;
} BiFontHeader;

typedef struct {
    uint16_t x;
    uint16_t y;
    uint16_t w;
    uint16_t h;
    int16_t base_x;
    int16_t base_y;
    int16_t advance_x;
    int16_t advance_y;
    uint32_t codepoint;
    uint32_t reserved;
} BiGlyphLayout;

union _BiGlyphNode;
typedef union _BiGlyphNode BiGlyphNode;

union _BiGlyphNode {
  BiGlyphLayout* layouts[0x100];
  BiGlyphNode* nodes[0x100];
};

typedef struct {
  BiGlyphNode* table[0x100];
  BiTexture* texture;
  int font_size;
  int base_line;
  BiGlyphLayout *pool;
} BiFont;

extern BiFont* bi_font_init(BiFont* font, const char *buffer, int size);
extern BiFont* bi_font_init_with_file(BiFont* font, const char *filename);
extern int bi_font_line_x_to_index(const BiFont* font, const char* text, int x);
extern BiGlyphLayout* bi_font_get_glyph_layout(const BiFont* font,uint32_t codepoint);

#endif
