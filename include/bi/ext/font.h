#ifndef __BISMITE_EXT_FONT_H__
#define __BISMITE_EXT_FONT_H__

#include <stdint.h>
#include <stdbool.h>

struct _BiNode;
typedef struct _BiNode BiNode;

struct _BiTexture;
typedef struct _BiTexture BiTexture;

struct _BiGlyphLayout;
typedef struct _BiGlyphLayout BiGlyphLayout;

struct _BiFontAtlas;
typedef struct _BiFontAtlas BiFontAtlas;

typedef struct {
  uint32_t version;
  uint16_t font_size;
  int16_t height;
  int16_t ascent;
  int16_t descent;
  uint32_t glyph_count;
} BiFontHeader;

struct _BiGlyphLayout {
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
};

union _BiGlyphNode;
typedef union _BiGlyphNode BiGlyphNode;

union _BiGlyphNode {
  BiGlyphLayout* layouts[0xFF];
  BiGlyphNode* nodes[0xFF];
};

struct _BiFontAtlas {
  BiGlyphNode* table[0xFF];
  BiTexture* texture;
  int font_size;
  int base_line;
  BiGlyphLayout *_pool;
};

extern bool bi_load_font_layout(const char *buffer, int size, BiFontAtlas* font);
extern bool bi_load_font_layout_from_file(const char *filename, BiFontAtlas* font);

extern void bi_update_label(BiNode* node, const char* text, const BiFontAtlas* font, uint8_t r, uint8_t g, uint8_t b, uint8_t a );
extern void bi_update_color(BiNode* node, uint8_t r, uint8_t g, uint8_t b, uint8_t a );

#endif
