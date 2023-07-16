#include "common.h"
#include <bi/ext/action.h>

#define DURATION 300

BiNode* popup_shade;
BiNode* popup;

static bool close(BiContext* context,BiNode* n, int x, int y, int button, bool pressed)
{
  if( bi_node_get_opacity(n)==0xff && !pressed) {
    popup_shade->visible = false;
  }
  return true; // swallow
}

static void callback_end(BiAction* action, void* payload)
{
  bi_remove_action(action->node,action);
  free(action);
}

static bool open(BiContext* context,BiNode* n, int x, int y, int button, bool pressed)
{
  if(!pressed) {
    popup_shade->visible = true;

    bi_node_set_opacity(popup,0);
    bi_node_set_scale(popup,0.5,0.5);

    // Fade In
    BiAction* fade_in = (BiAction*)bi_action_fade_in_init( ALLOC(BiActionFade), DURATION);
    fade_in->on_finish = callback_end;
    bi_add_action(popup,fade_in);
    bi_action_start(fade_in);

    // Scale To
    BiAction* scale_to = (BiAction*)bi_action_scale_to_init(ALLOC(BiActionScale),DURATION,1.0,1.0);
    scale_to->on_finish = callback_end;
    bi_add_action(popup,scale_to);
    bi_action_start(scale_to);

    bi_node_set_on_click(popup, close);
  }
  return false; // through
}

int main(int argc, char* argv[])
{
  BiContext* context = bi_init_context(ALLOC(BiContext),480,320,0,false,__FILE__);
  print_info(context);

  // Layer
  BiLayer *layer = bi_layer_init(ALLOC(BiLayer));
  layer->root = make_sprite_with_anchor("assets/check.png",0,0);
  bi_add_layer(context,layer);

  // shade
  popup_shade = bi_node_init(ALLOC(BiNode));
  bi_node_set_size(popup_shade,context->w,context->h);
  bi_set_color(popup_shade->color_modulate, 0, 0, 0, 128);
  bi_node_add_node(layer->root,popup_shade);

  // popup
  popup = bi_node_init(ALLOC(BiNode));
  bi_node_set_size(popup,380,240);
  bi_node_set_anchor(popup,0.5,0.5);
  bi_node_set_position(popup,context->w/2,context->h/2);
  bi_set_color(popup->color_tint, 0, 0, 0x66, 0xFF);
  bi_node_add_node(popup_shade,popup);

  // content
  BiNode* img = make_sprite("assets/tester.png");
  bi_node_set_position(img,0,-50);
  bi_node_add_node(popup,img);
  // font
  BiTexture *font_texture = ALLOC(BiTexture);
  bi_texture_init_with_filename(font_texture,"assets/font.png",false);
  BiFontAtlas *fonts[4];
  fonts[0] = load_font_atlas("assets/font12.dat", font_texture),
  fonts[1] = load_font_atlas("assets/font12b.dat", font_texture);
  fonts[2] = load_font_atlas("assets/font14.dat", font_texture);
  fonts[3] = load_font_atlas("assets/font14b.dat", font_texture);
  for(int i=0; i<4; i++){
    BiNode* label = bi_node_init(ALLOC(BiNode));
    bi_node_set_anchor(label,0,0);
    bi_node_set_position( label, -popup->w/2+20, 20+i*14 );
    bi_set_color(label->color_modulate,0,0,0,0);
    bi_label_set_text(label,fonts[i], "The quick brown fox jumps over the lazy dog");
    bi_node_add_node(popup,label);
  }

  // hide
  popup_shade->visible = false;

  // assign textures to layer
  layer->textures[0] = layer->root->texture;
  layer->textures[1] = img->texture;
  layer->textures[2] = font_texture;

  //
  bi_node_set_on_click(layer->root, open);

  bi_start_run_loop(context);
  return 0;
}
