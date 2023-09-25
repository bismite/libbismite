#include <bi/ext/font.h>
#include <bi/ext/simple_unicode.h>
#include <bi/logger.h>
#include <bi/node.h>
#include <bi/util.h>
#include <bi/bi_sdl.h>
#include <stdlib.h>
#include <string.h>

static BiGlyphNode* make_glyph_node()
{
  BiGlyphNode* n = malloc(sizeof(BiGlyphNode));
  for(int i=0;i<0x100;i++) n->nodes[i] = NULL;
  return n;
}

static bool __bi_font_init__(SDL_RWops* rwops, BiFont* font)
{
  font->pool = NULL;
  for(int i=0;i<0x100;i++) { font->table[i] = NULL; }
  int64_t fsize = SDL_RWsize(rwops);
  BiFontHeader header;
  SDL_RWread(rwops,&header,sizeof(BiFontHeader),1);
  if(header.version!=2) return false;
  font->font_size = header.font_size;
  font->base_line = header.descent;
  if( (fsize - sizeof(BiFontHeader)) % sizeof(BiGlyphLayout) != 0 ) {
    LOG("wrong file size\n");
    return false;
  }
  if( header.glyph_count != (fsize - sizeof(BiFontHeader)) / sizeof(BiGlyphLayout) ) {
    LOG("wrong file size\n");
    return false;
  }
  font->pool = malloc(sizeof(BiGlyphLayout)*header.glyph_count);
  size_t glyph_count = SDL_RWread(rwops,font->pool,sizeof(BiGlyphLayout),header.glyph_count);
  if(glyph_count!=header.glyph_count){
    LOG("wrong glyph count %zu (require %d)\n",glyph_count,header.glyph_count);
    return false;
  }
  for(int i=0; i<header.glyph_count; i++){
    BiGlyphLayout *l = &font->pool[i];
    uint32_t cp = l->codepoint;
    // uint8_t d = cp>>24 & 0xff;
    uint8_t c = cp>>16 & 0xff;
    uint8_t b = cp>>8  & 0xff;
    uint8_t a = cp     & 0xff;
    if( font->table[c] == NULL ) font->table[c] = make_glyph_node();
    if( font->table[c]->nodes[b] == NULL ) font->table[c]->nodes[b] = make_glyph_node();
    if( font->table[c]->nodes[b]->layouts[a] == NULL ) font->table[c]->nodes[b]->layouts[a] = l;
  }
  return true;
}

BiFont* bi_font_init(BiFont* font, const char *buffer, int size)
{
  SDL_RWops *rwops = SDL_RWFromConstMem(buffer,size);
  __bi_font_init__(rwops,font);
  SDL_RWclose(rwops);
  return font;
}

BiFont* bi_font_init_with_file(BiFont* font, const char *filename)
{
  SDL_RWops *rwops = SDL_RWFromFile(filename,"rb");
  __bi_font_init__(rwops,font);
  SDL_RWclose(rwops);
  return font;
}

BiGlyphLayout* bi_font_get_glyph_layout(const BiFont* font,uint32_t codepoint)
{
  // uint8_t d = cp>>24 & 0xff;
  uint8_t c = codepoint>>16 & 0xff;
  uint8_t b = codepoint>>8  & 0xff;
  uint8_t a = codepoint     & 0xff;
  if( font->table[c] == NULL ) return NULL;
  if( font->table[c]->nodes[b] == NULL ) return NULL;
  return font->table[c]->nodes[b]->layouts[a];
}

int bi_font_line_x_to_index(const BiFont* font, const char* text, int x)
{
  size_t textlen = strlen(text);
  int cursor = 0;
  int count = 0;
  BiGlyphLayout *glyph = NULL;
  if(x<0) return -1;
  while (textlen > 0) {
    glyph = bi_font_get_glyph_layout(font,utf8_getch_as_codepoint(&text, &textlen));
    count++;
    if(glyph==NULL) continue;
    if(glyph->codepoint==0) continue;
    if(glyph->w==0 && glyph->h==0 && glyph->advance_x==0 && glyph->advance_y==0 ) continue;
    if( cursor+glyph->advance_x >= x ) return count-1;
    cursor += glyph->advance_x;
  }
  return count;
}

void bi_font_get_label_size(const BiFont* font, const char* text, int *w, int *h)
{
  size_t textlen = strlen(text);
  int x = 0;
  BiGlyphLayout *glyph = NULL;
  while (textlen > 0) {
    glyph = bi_font_get_glyph_layout(font,utf8_getch_as_codepoint(&text, &textlen));
    if(glyph==NULL) continue;
    if(glyph->codepoint==0) continue;
    if(glyph->w==0 && glyph->h==0 && glyph->advance_x==0 && glyph->advance_y==0 ) continue;
    x += glyph->advance_x;
  }
  *w = x;
  *h = font->font_size;
}
