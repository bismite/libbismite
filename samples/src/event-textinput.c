#include "common.h"

BiLabel* labels[3];

static bool on_textinput(BiContext* context,BiNode* n, char* text)
{
  char buf[256];
  snprintf(buf, 256, "Text: %s", text);
  bi_label_set_text(labels[1],buf);
  snprintf(buf, 256, "Text Length: %ld", (long)strlen(text) );
  bi_label_set_text(labels[2],buf);
  return true;
}

int main(int argc,char* argv[])
{
  BiContext* context = bi_init_context(ALLOC(BiContext), 480, 320, 0, false, __FILE__);
  print_info(context);

  // root node
  BiNode* root = bi_node_init(ALLOC(BiNode));

  // font & labels
  const char* texts[3] = { "PRESS ANY KEY", "Text:", "Text Length:" };
  BiFontAtlas *font = load_font();
  for(int i=0;i<3;i++){
    BiLabel *label = bi_label_init(ALLOC(BiLabel),font);
    bi_label_set_background_color(label, RGBA(0x33,0,0,0xff) );
    bi_label_set_text(label, texts[i]);
    labels[i] = label;
    bi_node_set_position( (BiNode*)label, 10, context->h - 100 - i * 32 );
    bi_node_add_node(root,(BiNode*)label);
  }

  // set callbacks
  bi_node_set_on_textinput(root, on_textinput);

  // layer
  BiLayer *layer = malloc(sizeof(BiLayer));
  bi_layer_init(layer);
  bi_add_layer(context,layer);
  layer->root = root;
  layer->textures[0] = font->texture;

  // fps layer
  add_fps_layer(context,font);

  bi_start_run_loop(context);
  return 0;
}
