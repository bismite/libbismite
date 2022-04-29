#include "common.h"
#include <bi/bi_sdl.h>

static bool on_keyinput(BiContext* context,BiNode* node, uint16_t scancode, uint32_t keycode, uint16_t mod, bool pressed)
{
  BiNode* labels = node->userdata;
  char buf[256];
  snprintf(buf, 256, "Keycode: %s (%d)", SDL_GetKeyName(keycode), keycode);
  bi_label_set_text(&labels[1],labels[1].userdata,buf);
  snprintf(buf, 256, "KeycodeFromScancode: %s (%d)", SDL_GetKeyName(SDL_GetKeyFromScancode(scancode)),
                                                     SDL_GetKeyFromScancode(scancode) );
  bi_label_set_text(&labels[2],labels[2].userdata, buf);
  snprintf(buf, 256, "Scancode: %s (%d)", SDL_GetScancodeName(scancode), scancode );
  bi_label_set_text(&labels[3],labels[3].userdata, buf);
  snprintf(buf, 256, "MOD: %d", mod );
  bi_label_set_text(&labels[4],labels[4].userdata, buf);
  snprintf(buf, 256, "Pressed: %s", pressed?"DOWN":"UP" );
  bi_label_set_text(&labels[5],labels[5].userdata, buf);
  return false;
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
  BiNode* labels = malloc(sizeof(BiNode)*6);
  for(int i=0;i<6;i++){
    BiNode *label = &labels[i];
    bi_node_init(label);
    bi_node_set_scale(label,1.0,1.0);
    bi_node_set_position( label, 10, context->h - 100 - i * 32 );
    label->userdata = font;
    bi_node_add_node(root,label);
  }
  root->userdata = labels;

  const char* texts[] = {
    "PRESS ANY KEY",
    "Keycode:",
    "KeycodeFromScancode:",
    "Scancode:",
    "MOD:",
    "Pressed:"
  };
  for(int i=0;i<6;i++){
    bi_label_set_text(&labels[i],font, texts[i]);
  }
  // set callback
  bi_node_set_on_keyinput(root, on_keyinput);

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
