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
  BiContext* context = make_context(__FILE__);

  // Layer
  BiShaderNode *shader_node = bi_shader_node_init(ALLOC(BiShaderNode));
  BiNode *root = bi_shader_node_add_node(shader_node,make_bg("assets/check.png"));
  bi_add_shader_node(context,shader_node);

  // shade
  popup_shade = bi_node_init(ALLOC(BiNode));
  bi_node_set_size(popup_shade,context->w,context->h);
  popup_shade->color = RGBA(0, 0, 0, 128);
  bi_node_add_node(root,popup_shade);

  // popup
  popup = bi_node_init(ALLOC(BiNode));
  bi_node_set_size(popup,380,240);
  bi_node_set_anchor(popup,0.5,0.5);
  bi_node_set_position(popup,context->w/2,context->h/2);
  popup->tint = RGBA(0, 0, 0x66, 0xFF);
  bi_node_add_node(popup_shade,popup);

  // content
  BiNode* img = make_sprite("assets/tester.png");
  bi_node_set_position(img,0,-50);
  bi_node_add_node(popup,img);
  // font
  BiTexture *font_texture = ALLOC(BiTexture);
  bi_texture_init_with_filename(font_texture,"assets/font.png",false);
  char* fonts[4] = {
    "assets/font12.dat", "assets/font12b.dat", "assets/font14.dat", "assets/font14b.dat"
  };
  for(int i=0; i<4; i++){
    BiFont* f = load_font( fonts[i], font_texture );
    BiLabel* label = bi_label_init(ALLOC(BiLabel), f);
    bi_label_set_background_color(label,RGBA(0xff,0,0,64));
    bi_node_set_anchor((BiNode*)label,0,0);
    bi_node_set_position( (BiNode*)label, -popup->w/2+20, 20+i*14 );
    bi_label_set_text(label, "The quick brown fox jumps over the lazy dog");
    bi_node_add_node(popup,(BiNode*)label);
  }

  // hide
  popup_shade->visible = false;

  // assign textures to shader_node
  shader_node->textures[0] = root->texture;
  shader_node->textures[1] = img->texture;
  shader_node->textures[2] = font_texture;

  //
  bi_node_set_on_click(root, open);

  bi_start_run_loop(context);
  return 0;
}
