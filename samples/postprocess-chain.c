#include "common.h"

int main(int argc, char* argv[])
{
  BiContext* context = bi_init_context(ALLOC(BiContext), 480, 320, 0, true, __FILE__);
  print_info(context);

  // Shaders
  BiShader* shaders[] = {
    create_shader_with_default_vertex_shader("assets/shaders/postprocess-distortion.frag"),
    create_shader_with_default_vertex_shader("assets/shaders/postprocess-yellow.frag"),
    create_shader_with_default_vertex_shader("assets/shaders/postprocess-chop.frag"),
    create_shader_with_default_vertex_shader("assets/shaders/postprocess-negative.frag"),
    create_shader_with_default_vertex_shader("assets/shaders/postprocess-blur.frag"),
    NULL
  };

  // Main Layer
  BiLayer *layer = bi_layer_init(ALLOC(BiLayer));
  layer->root = make_sprite("assets/check.png");
  bi_node_set_position(layer->root,context->w/2,context->h/2);
  BiNode* face = make_sprite("assets/face01.png");
  bi_node_add_node(layer->root,face);
  layer->textures[0] = layer->root->texture;
  layer->textures[1] = face->texture;
  bi_add_layer(context,layer);

  // PostProcess Layers
  for(int i=0; shaders[i]!=NULL; i++){
    BiLayer *pp_layer = bi_layer_init_as_postprocess(ALLOC(BiLayer));
    BiFramebuffer *fb = &context->layers.framebuffer;
    pp_layer->shader = shaders[i];
    // one large rectangle for postprocess
    pp_layer->root = bi_node_init(ALLOC(BiNode));
    bi_node_set_size(pp_layer->root,context->w,context->h);
    // texture from framebuffer
    BiTexture *fb_tex = bi_texture_init_with_framebuffer(ALLOC(BiTexture),fb);
    bi_node_set_texture(pp_layer->root, fb_tex, 0,0,fb_tex->w,fb_tex->h);
    // framebuffer texture is upside-down
    pp_layer->root->texture_flip_vertical = true;
    // assign textures
    pp_layer->textures[0] = fb_tex;
    pp_layer->textures[1] = face->texture;
    bi_add_layer(context,pp_layer);
  }

  // fps layer
  BiFontAtlas *font = load_font();
  add_fps_layer(context,font);

  //
  bi_start_run_loop(context);
  return 0;
}
