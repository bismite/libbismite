#include "common.h"
#include <bi/bi_sdl.h>

BiLabel* labels[6];

static bool on_keyinput(BiContext* context,BiNode* node, uint16_t scancode, uint32_t keycode, uint16_t mod, bool pressed)
{
  char buf[256];
  snprintf(buf, 256, "Keycode: %s (%d)", SDL_GetKeyName(keycode), keycode);
  bi_label_set_text(labels[1],buf);
  snprintf(buf, 256, "KeycodeFromScancode: %s (%d)", SDL_GetKeyName(SDL_GetKeyFromScancode(scancode)),
                                                     SDL_GetKeyFromScancode(scancode) );
  bi_label_set_text(labels[2],buf);
  snprintf(buf, 256, "Scancode: %s (%d)", SDL_GetScancodeName(scancode), scancode );
  bi_label_set_text(labels[3],buf);
  snprintf(buf, 256, "MOD: %d", mod );
  bi_label_set_text(labels[4],buf);
  snprintf(buf, 256, "Pressed: %s", pressed?"DOWN":"UP" );
  bi_label_set_text(labels[5],buf);
  return false;
}

int main(int argc,char* argv[])
{
  BiContext* context = bi_init_context(ALLOC(BiContext), 480, 320, 0, false, __FILE__);
  print_info(context);

  // root node
  BiNode* root = bi_node_init(ALLOC(BiNode));

  // font & labels
  const char* texts[] = {
    "PRESS ANY KEY",
    "Keycode:",
    "KeycodeFromScancode:",
    "Scancode:",
    "MOD:",
    "Pressed:"
  };
  BiFontAtlas *font = load_font();
  for(int i=0;i<6;i++){
    BiLabel *l = bi_label_init(ALLOC(BiLabel),font);
    bi_label_set_background_color(l,RGBA(0x33,0,0,0xff));
    bi_label_set_text(l, texts[i]);
    labels[i] = l;
    bi_node_set_position( (BiNode*)l, 10, context->h - 100 - i * 32 );
    bi_node_add_node(root,(BiNode*)l);
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
