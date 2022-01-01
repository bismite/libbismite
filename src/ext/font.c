#include <bi/ext/font.h>
#include <bi/ext/simple_unicode.h>
#include <bi/node.h>
#include <bi/util.h>
#include <bi/bi_sdl.h>
#include <stdlib.h>
#include <string.h>

static BiGlyphNode* make_glyph_node()
{
  BiGlyphNode* n = malloc(sizeof(BiGlyphNode));
  for(int i=0;i<0xFF;i++) n->nodes[i] = NULL;
  return n;
}

static void bi_load_font_layout_from_rwops(SDL_RWops* rwops, BiFontAtlas* font)
{
  BiFontHeader header;
  SDL_RWread(rwops,&header,sizeof(BiFontHeader),1);
  font->font_size = header.font_size;
  font->base_line = header.descent;
  font->_pool = malloc(sizeof(BiGlyphLayout)*header.glyph_count);
  for(int i=0;i<0xFF;i++) { font->table[i] = NULL; }
  for(int i=0; i<header.glyph_count; i++){
    BiGlyphLayout *l = &font->_pool[i];
    size_t count = SDL_RWread(rwops,l,sizeof(BiGlyphLayout),1);
    if(count < 1) break;
    uint32_t cp = l->codepoint;
    // uint8_t d = cp>>24 & 0xff;
    uint8_t c = cp>>16 & 0xff;
    uint8_t b = cp>>8  & 0xff;
    uint8_t a = cp     & 0xff;
    if( font->table[c] == NULL ) font->table[c] = make_glyph_node();
    if( font->table[c]->nodes[b] == NULL ) font->table[c]->nodes[b] = make_glyph_node();
    if( font->table[c]->nodes[b]->layouts[a] == NULL ) font->table[c]->nodes[b]->layouts[a] = l;
  }
}


void bi_load_font_layout(const char *buffer, int size, BiFontAtlas* font)
{
  SDL_RWops *rwops = SDL_RWFromConstMem(buffer,size);
  bi_load_font_layout_from_rwops(rwops,font);
  SDL_RWclose(rwops);
}

void bi_load_font_layout_from_file(const char *filename, BiFontAtlas* font)
{
  SDL_RWops *rwops = SDL_RWFromFile(filename,"rb");
  bi_load_font_layout_from_rwops(rwops,font);
  SDL_RWclose(rwops);
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

void bi_update_label(BiNode* node, const char* text, const BiFontAtlas* font,
                     uint8_t r, uint8_t g, uint8_t b, uint8_t a  )
{
  // text
  size_t textlen = strlen(text);
  int x = 0;
  int y = font->base_line;
  int line_height = font->font_size;
  int i = 0;
  for(int i=0;i<node->children.size;i++) {
    bi_node_child_at(node,i)->visible = false;
  }
  while (textlen > 0) {
      uint32_t cp = utf8_getch_as_codepoint(&text, &textlen);
      BiGlyphLayout *glyph = get_glyph(font,cp);
      if(glyph==NULL) continue;
      if(glyph->codepoint==0) continue;
      if(glyph->w==0 && glyph->h==0 && glyph->advance_x==0 && glyph->advance_y==0 ) continue;

      BiNode* n = NULL;
      if( i < node->children.size ){
        n = bi_node_child_at(node,i);
      }else{
        n = malloc(sizeof(BiNode));
        bi_node_init(n);
        bi_node_add_node(node,n);
        n->texture_mapping = malloc(sizeof(BiTextureMapping));
        bi_texture_mapping_init(n->texture_mapping,NULL);
      }
      // node
      bi_node_set_position(n, x + glyph->base_x, y + glyph->base_y );
      bi_node_set_size(n,glyph->w,glyph->h);
      n->visible = true;
      //texture
      n->texture_mapping->texture = font->texture;
      bi_texture_mapping_set_bound(n->texture_mapping, glyph->x,glyph->y,glyph->w,glyph->h);
      // color
      bi_set_color( n->color, r,g,b,a );

      x += glyph->advance_x;
      i++;
  }
  bi_node_set_size(node,x,line_height);
  bi_node_set_matrix_include_anchor_translate(node,true);
}

void bi_update_color(BiNode* node, uint8_t r, uint8_t g, uint8_t b, uint8_t a )
{
  for(int i=0;i<node->children.size;i++) {
    bi_set_color( bi_node_child_at(node,i)->color, r,g,b,a );
  }
}
