#include <bi/bismite.h>
#include <bi/ext/font.h>
#include <stdio.h>
#include <stdlib.h>

static bool label_fps_indicate(BiContext* context, BiTimer* timer)
{
  BiNode *node = timer->userdata;
  BiFontAtlas *font = node->userdata;
  char text[1024];
  snprintf(text,1024,"FPS:%.2f CB:%ld RE:%ld ma:%ld OT:%.2f / %d,%d / %d,%d",
    context->profile.stats.fps,
    (long)context->profile.stats.time_spent_on_callback,
    (long)context->profile.stats.time_spent_on_rendering,
    (long)context->profile.stats.matrix_updated,

    context->profile.stats.fps == 0 ? 0 : 1000.0 / context->profile.stats.fps - context->profile.stats.time_spent_on_callback - context->profile.stats.time_spent_on_rendering,

    context->profile.on_update_callbacks_size,
    context->profile.callback_planned_nodes_size,

    context->profile.matrix_updated,
    context->profile.rendering_nodes_queue_size
  );
  bi_update_label(node, text, font, 0xff,0xff,0xff,0xff);
  return true;
}

static BiFontAtlas* __attribute__((unused))load_font()
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
    BiNode* label = malloc(sizeof(BiNode));
    bi_node_init(label);
    label->userdata = font;
    bi_set_color(label->color, 0,0,0,0xFF);
    bi_update_label(label, "FPS:00.00", font, 0xff,0xff,0xff,0xff);

    // timer
    BiTimer *timer = malloc(sizeof(BiTimer));
    bi_timer_init(timer, label_fps_indicate, 100, -1, label);
    bi_add_timer(&label->timers,timer);

    return label;
}

static void __attribute__((unused))add_fps_layer(BiContext* context,BiFontAtlas *font)
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
