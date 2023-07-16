#include "common.h"

static bool on_textinput(BiContext* context,BiNode* n, char* text)
{
  BiNode* labels = n->userdata;
  char buf[256];
  snprintf(buf, 256, "Text: %s", text);
  bi_label_set_text(&labels[1],labels[1].userdata,buf);
  snprintf(buf, 256, "Text Length: %ld", (long)strlen(text) );
  bi_label_set_text(&labels[2],labels[2].userdata,buf);
  return true;
}

int main(int argc,char* argv[])
{
  BiContext* context = bi_init_context(ALLOC(BiContext), 480, 320, 0, false, __FILE__);
  print_info(context);

  // root node
  BiNode* root = bi_node_init(ALLOC(BiNode));

  // font & labels
  BiFontAtlas *font = load_font();
  BiNode* labels = malloc(sizeof(BiNode)*3);
  for(int i=0;i<3;i++){
    BiNode *label = &labels[i];
    bi_node_init(label);
    bi_set_color(label->color_modulate, 0,0,0,0xff);
    bi_node_set_position( label, 10, context->h - 100 - i * 32 );
    label->userdata = font;
    bi_node_add_node(root,label);
  }
  root->userdata = labels;

  bi_label_set_text(&labels[0],font, "PRESS ANY KEY");
  bi_label_set_text(&labels[1],font, "Text:");
  bi_label_set_text(&labels[2],font, "Text Length:");

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
