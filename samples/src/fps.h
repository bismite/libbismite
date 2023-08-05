#include <bi/bismite.h>
#include <bi/ext/label.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
  BiContext* context;
  BiNode *node;
} FPSCallbackData;

static void label_fps_indicate(BiTimer* timer,double dt)
{
  FPSCallbackData* dat = timer->userdata;
  BiNode *node = dat->node;
  BiLabel *label = (BiLabel*)node;
  BiContext *context = dat->context;
  char text[1024];
  snprintf(text,1024,"FPS:%.2f", context->profile.stats.fps );
  bi_label_set_text(label, text);
}

__attribute__((unused)) static BiFontAtlas* load_font()
{
  // texture
  BiTexture *font_texture = malloc(sizeof(BiTexture));
  bi_texture_init_with_filename(font_texture,"assets/font.png",false);
  // layout
  BiFontAtlas *font = malloc(sizeof(BiFontAtlas));
  const char* layout_file_name = "assets/font12.dat";
  bi_font_init_with_file(font,layout_file_name);
  font->texture = font_texture;
  return font;
}

static BiLabel* create_fps_label(BiContext* context, BiFontAtlas *font)
{
  // label
  BiLabel* label = bi_label_init(malloc(sizeof(BiLabel)),font);
  bi_label_set_background_color(label, RGBA(0,0,0,128));
  bi_label_set_text(label, "FPS:00.00");
  // timer
  BiTimer *timer = malloc(sizeof(BiTimer));
  FPSCallbackData *dat = malloc(sizeof(FPSCallbackData));
  dat->context = context;
  dat->node = (BiNode*)label;
  bi_timer_init(timer, label_fps_indicate, 100, -1, dat);
  bi_node_add_timer((BiNode*)label,timer);
  return label;
}

__attribute__((unused)) static void add_fps_layer(BiContext* context,BiFontAtlas *font)
{
    // Layer Group
    BiLayerGroup* layer_group = malloc(sizeof(BiLayerGroup));
    bi_layer_group_init(layer_group);
    // layer
    BiLayer *layer = malloc(sizeof(BiLayer));
    bi_layer_init(layer);
    layer->textures[0] = font->texture;
    // label
    BiNode* label = (BiNode*)create_fps_label(context,font);
    bi_node_set_anchor(label,0,1);
    bi_node_set_position(label,0,context->h);
    layer->root = label;
    bi_layer_group_add_layer(layer_group,layer);
    bi_layer_group_add_layer_group(&context->layers,layer_group);
}
