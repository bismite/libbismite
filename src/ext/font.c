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

static bool __bi_font_init__(SDL_RWops* rwops, BiFontAtlas* font)
{
  font->_pool = NULL;
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
  font->_pool = malloc(sizeof(BiGlyphLayout)*header.glyph_count);
  size_t glyph_count = SDL_RWread(rwops,font->_pool,sizeof(BiGlyphLayout),header.glyph_count);
  if(glyph_count!=header.glyph_count){
    LOG("wrong glyph count %zu (require %d)\n",glyph_count,header.glyph_count);
    return false;
  }
  for(int i=0; i<header.glyph_count; i++){
    BiGlyphLayout *l = &font->_pool[i];
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

BiFontAtlas* bi_font_init(BiFontAtlas* font, const char *buffer, int size)
{
  SDL_RWops *rwops = SDL_RWFromConstMem(buffer,size);
  __bi_font_init__(rwops,font);
  SDL_RWclose(rwops);
  return font;
}

BiFontAtlas* bi_font_init_with_file(BiFontAtlas* font, const char *filename)
{
  SDL_RWops *rwops = SDL_RWFromFile(filename,"rb");
  __bi_font_init__(rwops,font);
  SDL_RWclose(rwops);
  return font;
}

static BiGlyphLayout* get_glyph(const BiFontAtlas* font,uint32_t cp)
{
  // uint8_t d = cp>>24 & 0xff;
  uint8_t c = cp>>16 & 0xff;
  uint8_t b = cp>>8  & 0xff;
  uint8_t a = cp     & 0xff;
  if( font->table[c] == NULL ) return NULL;
  if( font->table[c]->nodes[b] == NULL ) return NULL;
  return font->table[c]->nodes[b]->layouts[a];
}

int bi_font_line_x_to_index(const BiFontAtlas* font, const char* text, int x)
{
  size_t textlen = strlen(text);
  int cursor = 0;
  int count = 0;
  BiGlyphLayout *glyph = NULL;
  if(x<0) return -1;
  while (textlen > 0) {
    glyph = get_glyph(font,utf8_getch_as_codepoint(&text, &textlen));
    count++;
    if(glyph==NULL) continue;
    if(glyph->codepoint==0) continue;
    if(glyph->w==0 && glyph->h==0 && glyph->advance_x==0 && glyph->advance_y==0 ) continue;
    if( cursor+glyph->advance_x >= x ) return count-1;
    cursor += glyph->advance_x;
  }
  return count;
}

void bi_font_get_label_size(const BiFontAtlas* font, const char* text, int *w, int *h)
{
  size_t textlen = strlen(text);
  int x = 0;
  BiGlyphLayout *glyph = NULL;
  while (textlen > 0) {
    glyph = get_glyph(font,utf8_getch_as_codepoint(&text, &textlen));
    if(glyph==NULL) continue;
    if(glyph->codepoint==0) continue;
    if(glyph->w==0 && glyph->h==0 && glyph->advance_x==0 && glyph->advance_y==0 ) continue;
    x += glyph->advance_x;
  }
  *w = x;
  *h = font->font_size;
}

static inline BiNode* get_label_node(BiNode* node)
{
  BiNode* label = NULL;
  if(node->children.size == 0){
    label = bi_node_init(malloc(sizeof(BiNode)));
    bi_node_add_node(node,label);
  } else {
    label = bi_node_child_at(node,0);
  }
  return label;
}

void bi_label_set_text_with_color(BiNode* node, const BiFontAtlas* font, const char* text,
                                  uint8_t r, uint8_t g, uint8_t b, uint8_t a, float opacity)
{
  // text
  size_t textlen = strlen(text);
  int x = 0;
  int y = font->base_line;
  int line_height = font->font_size;
  int child_index = 0;
  // label
  BiNode* label = get_label_node(node);
  // hide
  for(int i=0;i<label->children.size;i++) {
    bi_node_child_at(label,i)->visible = false;
  }
  // write
  while (textlen > 0) {
    uint32_t cp = utf8_getch_as_codepoint(&text, &textlen);
    BiGlyphLayout *glyph = get_glyph(font,cp);
    if(glyph==NULL) continue;
    if(glyph->codepoint==0) continue;
    if(glyph->w==0 && glyph->h==0 && glyph->advance_x==0 && glyph->advance_y==0 ) continue;
    // character node
    BiNode* n = NULL;
    if( child_index < label->children.size ){
      n = bi_node_child_at(label,child_index);
    }else{
      n = bi_node_init(malloc(sizeof(BiNode)));
      bi_node_add_node(label,n);
    }
    // node geometry
    bi_node_set_position(n, x + glyph->base_x, y + glyph->base_y );
    bi_node_set_size(n,glyph->w,glyph->h);
    n->visible = true;
    // texture and color
    bi_node_set_texture(n,font->texture,glyph->x,glyph->y,glyph->w,glyph->h);
    bi_set_color( n->color, r,g,b,a );
    n->opacity = opacity;
    //
    x += glyph->advance_x;
    child_index++;
  }
  bi_node_set_size(node,x,line_height);
  // set position to zero
  bi_node_set_position(label, -node->_w*node->_anchor_x, -node->_h*node->_anchor_y);
}

void bi_label_set_text(BiNode* label, const BiFontAtlas* font, const char* text)
{
  bi_label_set_text_with_color(label,font,text,0,0,0,0,1.0);
}

void bi_label_set_color(BiNode* node, uint8_t r, uint8_t g, uint8_t b, uint8_t a, float opacity )
{
  BiNode* label = get_label_node(node);
  BiNode* n = NULL;
  for(int i=0;i<label->children.size;i++) {
    n = bi_node_child_at(label,i);
    bi_set_color( n->color, r,g,b,a );
    n->opacity = opacity;
  }
}

void bi_label_set_color_with_range(BiNode* node, int start, int end,
                                   uint8_t r, uint8_t g, uint8_t b, uint8_t a, float opacity )
{
  BiNode* label = get_label_node(node);
  BiNode* n = NULL;
  if(start<0) start = 0;
  if(end>label->children.size) end = label->children.size;
  if(start>end) return;
  for(int i=start;i<end;i++) {
    n = bi_node_child_at(label,i);
    bi_set_color( n->color, r,g,b,a );
    n->opacity = opacity;
  }
}
