#include "common.h"
#include <bi/ext/action.h>

#define DURATION 300

BiNode* popup_shade;
BiNode* popup;

static BiAction* alloc_action(size_t size, void* func, void* payload)
{
  BiAction* action = malloc(sizeof(BiAction));
  bi_action_init(action);
  action->action_data = malloc(size);
  action->on_finish = func;
  action->on_finish_callback_context = payload;
  return action;
}

static bool close(BiContext* context,BiNode* n, int x, int y, int button, bool pressed)
{
  if(n->opacity==1.0 && !pressed) {
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

    popup->opacity=0;
    bi_node_set_scale(popup,0.5,0.5);

    // Fade In
    BiAction* fade_in = alloc_action( sizeof(BiActionFade), callback_end, NULL );
    bi_action_fade_in_init(fade_in,DURATION);
    bi_add_action(popup,fade_in);
    bi_action_start(fade_in);

    // Scale To
    BiAction* scale_to = alloc_action( sizeof(BiActionScale), callback_end, NULL );
    bi_action_scale_to_init(scale_to,DURATION,1.0,1.0);
    bi_add_action(popup,scale_to);
    bi_action_start(scale_to);

    bi_node_set_on_click(popup, close);
  }
  return false; // through
}

int main(int argc, char* argv[])
{
  BiContext* context = malloc(sizeof(BiContext));
  bi_init_context(context, 480, 320, 60, false, __FILE__);
  print_info(context);

  // layer
  BiLayer *layer = malloc(sizeof(BiLayer));
  bi_layer_init(layer);

  // root node
  BiNode* root = malloc(sizeof(BiNode));
  bi_node_init(root);
  layer->root = root;

  BiNode* bg = make_sprite("assets/check.png");
  bi_node_set_position(bg,context->w/2,context->h/2);
  bi_node_add_node(root,bg);

  // shade
  popup_shade = bi_node_init(malloc(sizeof(BiNode)));
  bi_node_set_size(popup_shade,context->w,context->h);
  bi_set_color(popup_shade->color, 0, 0, 0, 0);
  bi_node_add_node(root,popup_shade);

  // popup
  popup = bi_node_init(malloc(sizeof(BiNode)));
  bi_node_set_size(popup,380,240);
  bi_node_set_anchor(popup,0.5,0.5);
  bi_node_set_position(popup,context->w/2,context->h/2);
  bi_set_color(popup->color, 0, 0, 0x66, 0xFF);
  bi_node_add_node(popup_shade,popup);

  // content
  BiNode* img = make_sprite("assets/tile.png");
  bi_node_set_position(img,0,-50);
  bi_node_add_node(popup,img);
  // font
  BiTexture *font_texture = malloc(sizeof(BiTexture));
  bi_texture_init_with_filename(font_texture,"assets/mixed.png",false);
  BiFontAtlas *fonts[4];
  fonts[0] = load_font_atlas("assets/small.dat", font_texture),
  fonts[1] = load_font_atlas("assets/small-bold.dat", font_texture);
  fonts[2] = load_font_atlas("assets/large.dat", font_texture);
  fonts[3] = load_font_atlas("assets/large-bold.dat", font_texture);
  for(int i=0; i<4; i++){
    BiNode* label = bi_node_init(malloc(sizeof(BiNode)));
    bi_node_set_anchor(label,0,0);
    bi_node_set_position( label, -popup->w/2+20, 10+i*14 );
    bi_update_label(label, "The quick brown fox jumps over the lazy dog", fonts[i], 0xFF,0xFF,0xFF,0xFF);
    bi_node_add_node(popup,label);
  }

  // hide
  popup_shade->visible = false;

  // assign textures to layer
  layer->textures[0] = bg->texture_mapping->texture;
  layer->textures[1] = img->texture_mapping->texture;
  layer->textures[2] = font_texture;

  //
  bi_node_set_on_click(root, open);

  bi_add_layer(context,layer);

  bi_start_run_loop(context);
  return 0;
}
