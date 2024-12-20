#include <bi/ext/label.h>
#include <bi/ext/font.h>
#include <bi/ext/simple_unicode.h>
#include <bi/logger.h>
#include <bi/node.h>
#include <bi/util.h>
#include <bi/bi_sdl.h>
#include <stdlib.h>
#include <string.h>

static inline BiNode* get_holder_node(BiLabel* label)
{
  if(label->_holder==NULL){
    label->_holder = bi_node_init(malloc(sizeof(BiNode)));
    bi_node_add_node(&label->node,label->_holder);
  }
  return label->_holder;
}

BiLabel* bi_label_init(BiLabel* label, BiFont* font)
{
  bi_node_init((BiNode*)label);
  label->font = font;
  label->_holder = NULL;
  label->color = RGBA32(0xFFFFFFFF);
  label->tint = RGBA32(0);
  bi_label_set_background_color(label,RGBA(0,0,0,0));
  return label;
}

void bi_label_deinit(BiLabel* label)
{
  label->font = NULL;
  BiNode* holder = label->_holder;
  if(holder==NULL) return;
  BiNode* n;
  for(int i=0; i<holder->children.size;i++) {
    n = bi_node_child_at(holder,i);
    bi_node_base_deinit((BiNodeBase*)n);
    free(n);
  }
  bi_node_base_deinit((BiNodeBase*)holder);
  free(holder);
  bi_node_base_deinit((BiNodeBase*)label);
}

void bi_label_set_text(BiLabel* label, const char* text)
{
  BiNode *node = (BiNode*)label;
  BiFont *font = label->font;
  // text
  size_t textlen = strlen(text);
  int x = 0;
  int y = font->base_line;
  int line_height = font->font_size;
  int child_index = 0;
  //
  BiNode* holder = get_holder_node(label);
  // hide
  for(int i=0;i<holder->children.size;i++) {
    ((BiNode*)bi_node_child_at(holder,i))->visible = false;
  }
  // write
  while (textlen > 0) {
    uint32_t cp = utf8_getch_as_codepoint(&text, &textlen);
    BiGlyphLayout *glyph = bi_font_get_glyph_layout(font,cp);
    if(glyph==NULL) continue;
    if(glyph->codepoint==0) continue;
    if(glyph->w==0 && glyph->h==0 && glyph->advance_x==0 && glyph->advance_y==0 ) continue;
    // character node
    BiNode* n = NULL;
    if( child_index < holder->children.size ){
      n = bi_node_child_at(holder,child_index);
    }else{
      n = bi_node_init(malloc(sizeof(BiNode)));
      bi_node_add_node(holder,n);
    }
    // node geometry
    bi_node_set_position(n, x + glyph->base_x, y + glyph->base_y );
    bi_node_set_size(n,glyph->w,glyph->h);
    n->visible = true;
    // texture and color
    bi_node_set_texture(n,font->texture,glyph->x,glyph->y,glyph->w,glyph->h);
    n->color = label->color;
    n->tint = label->tint;
    //
    x += glyph->advance_x;
    child_index++;
  }
  // resize
  bi_node_set_size(node,x,line_height);
  bi_label_anchor_reposition(label);
}

void bi_label_set_tint(BiLabel* label, BiColor color )
{
  label->tint = color;
  BiNode* holder = get_holder_node(label);
  BiNode* n = NULL;
  for(int i=0;i<holder->children.size;i++) {
    n = bi_node_child_at(holder,i);
    n->tint = color;
  }
}

void bi_label_set_color(BiLabel* label, BiColor color )
{
  label->color = color;
  BiNode* holder = get_holder_node(label);
  BiNode* n = NULL;
  for(int i=0;i<holder->children.size;i++) {
    n = bi_node_child_at(holder,i);
    n->color = color;
  }
}

void bi_label_set_tint_with_range(BiLabel* label, int start, int end, BiColor color)
{
  BiNode* holder = get_holder_node(label);
  BiNode* n = NULL;
  if(start<0) start = 0;
  if(end>holder->children.size) end = holder->children.size;
  if(start>end) return;
  for(int i=start;i<end;i++) {
    n = bi_node_child_at(holder,i);
    n->tint = color;
  }
}

void bi_label_set_color_with_range(BiLabel* label, int start, int end, BiColor color)
{
  BiNode* holder = get_holder_node(label);
  BiNode* n = NULL;
  if(start<0) start = 0;
  if(end>holder->children.size) end = holder->children.size;
  if(start>end) return;
  for(int i=start;i<end;i++) {
    n = bi_node_child_at(holder,i);
    n->color = color;
  }
}

void bi_label_anchor_reposition(BiLabel* label)
{
  BiNode* n = (BiNode*)label;
  BiNode* holder = get_holder_node(label);
  bi_node_set_position(holder, -n->w*n->anchor_x, -n->h*n->anchor_y);
}
