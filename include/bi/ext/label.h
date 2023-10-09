#ifndef __BISMITE_EXT_LABEL_H__
#define __BISMITE_EXT_LABEL_H__

#include <bi/node.h>
#include <bi/ext/font.h>
#include <bi/color.h>

typedef struct BiLabel{
  BiNode node;
  BiFont* font;
  BiNode* _holder;
  // colors
  BiColor _modulate;
  BiColor _tint;
} BiLabel;

extern BiLabel* bi_label_init(BiLabel* label, BiFont* font);
extern void bi_label_deinit(BiLabel* label);
extern void bi_label_set_text(BiLabel* label, const char* text);
extern void bi_label_set_tint_color(BiLabel* label, BiColor color );
extern void bi_label_set_modulate_color(BiLabel* label, BiColor color );
extern void bi_label_set_tint_color_with_range(BiLabel* label, int start, int end, BiColor color);
extern void bi_label_set_modulate_color_with_range(BiLabel* label, int start, int end, BiColor color);
static inline void bi_label_set_background_color(BiLabel* label, BiColor color){ label->node.color_modulate = color; }
extern void bi_label_anchor_reposition(BiLabel* label);

#endif
