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
  BiContext* context = make_context(__FILE__);
  // Font
  BiTexture *font_texture = bi_texture_init_with_filename(ALLOC(BiTexture),"assets/font.png",false);
  BiFont *font = load_font("assets/font12.dat",font_texture);
  // shader_node
  BiShaderNode *shader_node = bi_shader_node_init(ALLOC(BiShaderNode));
  BiNode* root = bi_node_add_node(shader_node,bi_node_init(ALLOC(BiNode)));
  bi_node_add_node(&context->default_framebuffer_node,shader_node);
  shader_node->textures[0] = font->texture;
  // font & labels
  const char* texts[3] = { "PRESS ANY KEY", "Text:", "Text Length:" };
  for(int i=0;i<3;i++){
    BiLabel *label = bi_label_init(ALLOC(BiLabel),font);
    bi_label_set_background_color(label, RGBA(0x33,0,0,0xff) );
    bi_label_set_text(label, texts[i]);
    labels[i] = label;
    bi_node_set_scale((BiNode*)label,2,2);
    bi_node_set_position( (BiNode*)label, 10, context->h - 100 - i * 32 );
    bi_node_add_node(root,(BiNode*)label);
  }
  // set callbacks
  bi_node_set_on_textinput(root, on_textinput);
  // start
  bi_start_run_loop(context);
  return 0;
}
