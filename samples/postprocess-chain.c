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

  // Main Framebuffer
  BiNode* canvas = bi_node_init(ALLOC(BiNode));
  bi_node_set_size(canvas,W,H);
  canvas->framebuffer = bi_framebuffer_init(ALLOC(BiFramebuffer),W,H);
  BiTexture* tex = bi_texture_init_with_framebuffer(ALLOC(BiTexture), canvas->framebuffer);
  bi_node_set_texture(canvas,tex,0,0,tex->w,tex->h);
  canvas->texture_flip_vertical = true;
  // Main Shader
  BiShaderNode *snode = bi_shader_node_init(ALLOC(BiShaderNode));
  BiNode* bg = bi_node_add_node(snode, make_bg("assets/check.png"));
  BiNode* face = bi_node_add_node(snode, make_sprite("assets/face01.png"));
  bi_node_set_scale(face,2,2);
  bi_node_set_position(face,W/2,H/2);
  snode->textures[0] = bi_node_get_texture(bg);
  snode->textures[1] = bi_node_get_texture(face);
  bi_node_add_node(canvas,snode);

  // Post Process
  BiNode* child = canvas;
  for(int i=0; shaders[i]!=NULL; i++){
    BiShaderNode *snode = bi_shader_node_init(ALLOC(BiShaderNode));
    snode->shader = shaders[i];
    snode->textures[0] = bi_node_get_texture(child);
    bi_node_add_node(snode,child);
    BiNode* fbnode = bi_node_init(ALLOC(BiNode));
    fbnode->framebuffer = bi_framebuffer_init(ALLOC(BiFramebuffer),W,H);
    fbnode->texture_flip_vertical = true;
    BiTexture* tex = bi_texture_init_with_framebuffer(ALLOC(BiTexture), fbnode->framebuffer);
    bi_node_set_texture(fbnode,tex,0,0,tex->w,tex->h);
    bi_node_set_size(fbnode,W,H);
    bi_node_add_node(fbnode,snode);
    //
    child = fbnode;
  }

  snode = bi_shader_node_init(ALLOC(BiShaderNode));
  bi_node_add_node(snode,child);
  snode->textures[0] = bi_node_get_texture(child);
  bi_node_add_node(&context->default_framebuffer_node,snode);

  // start
  bi_start_run_loop(context);
  return 0;
}
