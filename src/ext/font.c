#include <bi/ext/font.h>
#include <bi/ext/simple_unicode.h>
#include <bi/node.h>
#include <bi/util.h>
#include <stdlib.h>
#include <string.h>

static void bi_load_font_layout_from_rwops(SDL_RWops* rwops, BiFontAtlas* font)
{
  BiFontHeader header;

  SDL_RWread(rwops,&header,sizeof(BiFontHeader),1);

  font->font_size = header.font_size;
  font->base_line = header.descent;

  for(int i=0;i<0xFFFF;i++) {
    font->table[i].utf8 = 0; // XXX: no-font
  }

  for(int i=0; i<header.glyph_count; i++){
    BiGlyphLayout l;
    size_t count = SDL_RWread(rwops,&l,sizeof(BiGlyphLayout),1);
    if(count < 1) {
      break;
    }
    uint16_t ucs2 = utf8_to_ucs2( l.utf8 );
    font->table[ucs2] = l;
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
      uint32_t ucs2 = utf8_getch_as_ucs2(&text, &textlen);

      const BiGlyphLayout *glyph = &(font->table[ucs2]);

      if(glyph->utf8==0 || glyph->w==0 || glyph->h==0) {
        continue;
      }

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
