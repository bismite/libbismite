#ifndef __BISMITE_EXT_LABEL_H__
#define __BISMITE_EXT_LABEL_H__

#include <bi/node.h>
#include <bi/ext/font.h>
#include <bi/color.h>

typedef struct BiLabel{
  BiNode node;
  BiFontAtlas* font;
  // colors
  BiColor _background;
  BiColor _modulate;
  BiColor _tint;
} BiLabel;

extern BiLabel* bi_label_init(BiLabel* label, BiFontAtlas* font);
extern BiLabel* bi_label_set_text(BiLabel* label, const char* text);
extern BiLabel* bi_label_set_tint_color(BiLabel* label, BiColor color );
extern BiLabel* bi_label_set_modulate_color(BiLabel* label, BiColor color );
extern BiLabel* bi_label_set_background_color(BiLabel* label, BiColor color );
extern BiLabel* bi_label_set_color_with_range(BiLabel* label, int start, int end, BiColor tint, BiColor modulate );
extern BiLabel* bi_label_anchor_reposition(BiLabel* label);

#endif
