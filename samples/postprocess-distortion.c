#include "common.h"

int main(int argc, char* argv[])
{
  BiContext* context = make_context(__FILE__);
  // Shaders
  BiShader *pp_shader = create_shader_with_default_vertex_shader("assets/shaders/postprocess-distortion.frag");
  // Main Layer
  BiLayer *layer = bi_layer_init(ALLOC(BiLayer));
  BiNode *bg = bi_layer_add_node(layer,make_bg("assets/check.png"));
  BiNode* face = make_sprite("assets/face01.png");
  bi_node_set_position(face,context->w/2,context->h/2);
  bi_node_add_node(bg,face);
  layer->textures[0] = bg->texture;
  layer->textures[1] = face->texture;
  // PostProcess Layer
  BiLayer *pp_layer = bi_layer_init_as_postprocess(ALLOC(BiLayer));
  BiNode *root = bi_layer_add_node(pp_layer,bi_node_init(ALLOC(BiNode)));
  BiFramebuffer *fb = &context->layers.framebuffer;
  pp_layer->shader = pp_shader;
  // one large rectangle for postprocess
  bi_node_set_size(root,context->w,context->h);
  // texture from framebuffer
  BiTexture *fb_tex = bi_texture_init_with_framebuffer(ALLOC(BiTexture),fb);
  bi_node_set_texture(root, fb_tex, 0,0,fb_tex->w,fb_tex->h);
  // framebuffer texture is upside-down
  root->texture_flip_vertical = true;
  // assign textures
  pp_layer->textures[0] = fb_tex;
  pp_layer->textures[1] = face->texture;
  // layers
  bi_add_layer(context,layer);
  bi_add_layer(context,pp_layer);
  // start
  bi_start_run_loop(context);
  return 0;
}
