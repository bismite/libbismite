#include "common.h"

static bool on_textinput(BiContext* context,BiNode* n, char* text)
{
  BiNode* labels = n->userdata;
  BiNode* label;
  char buf[256];

  snprintf(buf, 256, "Text: %s", text);
  label = &labels[1];
  bi_update_label(label, buf, label->userdata,0xFF,0xFF,0xFF,0xFF);

  snprintf(buf, 256, "Text Length: %ld", (long)strlen(text) );
  label = &labels[2];
  bi_update_label(label, buf, label->userdata,0xFF,0xFF,0xFF,0xFF);

  return true;
}

int main(int argc,char* argv[])
{
  BiContext* context = malloc(sizeof(BiContext));
  bi_init_context(context, 480, 320, 0, false, __FILE__);
  print_info(context);

  // root node
  BiNode* root = malloc(sizeof(BiNode));
  bi_node_init(root);

  // font
  BiFontAtlas *font = load_font();

  // labels
  BiNode* labels = malloc(sizeof(BiNode)*3);
  for(int i=0;i<3;i++){
    BiNode *label = &labels[i];
    bi_node_init(label);
    label->scale_x = label->scale_y = 1.0;
    bi_node_set_position( label, 10, context->h - 100 - i * 32 );
    label->userdata = font;
    bi_node_add_node(root,label);
  }
  root->userdata = labels;

  bi_update_label(&labels[0], "PRESS ANY KEY", font,0xFF,0xFF,0xFF,0xFF);
  bi_update_label(&labels[1], "Text:", font,0xFF,0xFF,0xFF,0xFF);
  bi_update_label(&labels[2], "Text Length:", font,0xFF,0xFF,0xFF,0xFF);

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
