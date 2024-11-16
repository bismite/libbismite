#include "common.h"

int main(int argc, char* argv[])
{
  BiContext* context = make_context(__FILE__);
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
  BiShaderNode *shader_node = bi_shader_node_init(ALLOC(BiShaderNode));
  BiNode* root = bi_shader_node_add_node(shader_node, make_bg("assets/check.png"));
  BiNode* face = make_sprite("assets/face01.png");
  bi_node_set_scale(face,2,2);
  bi_node_set_position(face,context->w/2,context->h/2);
  bi_node_add_node(root,face);
  shader_node->textures[0] = root->texture;
  shader_node->textures[1] = face->texture;
  bi_add_shader_node(context,shader_node);
  // PostProcess Shader Nodes
  for(int i=0; shaders[i]!=NULL; i++){
    BiShaderNode *pp_shader_node = bi_shader_node_init_as_postprocess(ALLOC(BiShaderNode));
    BiNode* root = bi_shader_node_add_node(pp_shader_node,bi_node_init(ALLOC(BiNode)));
    BiFramebuffer *fb = &context->shader_nodes.framebuffer;
    pp_shader_node->shader = shaders[i];
    // one large rectangle for postprocess
    bi_node_set_size(root,context->w,context->h);
    // texture from framebuffer
    BiTexture *fb_tex = bi_texture_init_with_framebuffer(ALLOC(BiTexture),fb);
    bi_node_set_texture(root, fb_tex, 0,0,fb_tex->w,fb_tex->h);
    // framebuffer texture is upside-down
    root->texture_flip_vertical = true;
    // assign textures
    pp_shader_node->textures[0] = fb_tex;
    pp_shader_node->textures[1] = face->texture;
    bi_add_shader_node(context,pp_shader_node);
  }
  // start
  bi_start_run_loop(context);
  return 0;
}
