#ifndef __BISMITE_EXT_LABEL_H__
#define __BISMITE_EXT_LABEL_H__

#include <bi/node.h>
#include <bi/ext/font.h>
#include <bi/color.h>

typedef struct BiLabel{
  BiNode node;
  BiColor color;
  BiColor tint;
  BiFont* font;
  BiNode* _holder;
} BiLabel;

extern BiLabel* bi_label_init(BiLabel* label, BiFont* font);
extern void bi_label_deinit(BiLabel* label);
extern void bi_label_set_text(BiLabel* label, const char* text);
extern void bi_label_set_color(BiLabel* label, BiColor color );
extern void bi_label_set_tint(BiLabel* label, BiColor color );
extern void bi_label_set_tint_with_range(BiLabel* label, int start, int end, BiColor color);
extern void bi_label_set_color_with_range(BiLabel* label, int start, int end, BiColor color);
static inline void bi_label_set_background_color(BiLabel* label, BiColor color){ label->node.color = color; }
static inline void bi_label_set_background_tint(BiLabel* label, BiColor color){ label->node.tint = color; }
extern void bi_label_anchor_reposition(BiLabel* label);

#endif
