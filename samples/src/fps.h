#include <bi/bismite.h>
#include <bi/ext/font.h>
#include <stdio.h>
#include <stdlib.h>

static void label_fps_indicate(BiContext* context, BiTimer* timer,double dt)
{
  BiNode *node = timer->userdata;
  BiFontAtlas *font = node->userdata;
  char text[1024];
  snprintf(text,1024,"FPS:%.2f", context->profile.stats.fps );
  bi_update_label(node, text, font, 0xff,0xff,0xff,0xff);
}

__attribute__((unused)) static BiFontAtlas* load_font()
{
  // texture
  BiTexture *font_texture = malloc(sizeof(BiTexture));
  bi_texture_init_with_filename(font_texture,"assets/mixed.png",false);
  // layout
  BiFontAtlas *font = malloc(sizeof(BiFontAtlas));
  const char* layout_file_name = "assets/large-bold.dat";
  bi_load_font_layout_from_file(layout_file_name,font);
  font->texture = font_texture;
  return font;
}

static BiNode* create_fps_label(BiContext* context, BiFontAtlas *font)
{
    // label
    BiNode* label = bi_node_init(malloc(sizeof(BiNode)));
    label->userdata = font;
    bi_set_color(label->color, 0,0,0,0xFF);
    bi_update_label(label, "FPS:00.00", font, 0xff,0xff,0xff,0xff);

    // timer
    BiTimer *timer = malloc(sizeof(BiTimer));
    bi_timer_init(timer, label_fps_indicate, 100, -1, label);
    bi_node_add_timer(label,timer);

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
    BiNode* label = create_fps_label(context,font);
    label->anchor_x = 0;
    label->anchor_y = 1;
    label->x = 0;
    label->y = context->h;
    layer->root = label;
    bi_layer_group_add_layer(layer_group,layer);
    bi_layer_group_add_layer_group(&context->layers,layer_group);
}
