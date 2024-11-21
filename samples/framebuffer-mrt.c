#include "common.h"

// 200x200, Blue Background
BiNode* draw_canvas(BiContext* context)
{
  BiNode* canvas = bi_node_init(ALLOC(BiNode));
  bi_node_set_size(canvas,200,200);
  canvas->framebuffer = bi_framebuffer_init_with_texture_num(ALLOC(BiFramebuffer),canvas->w,canvas->h,2);
  BiNode *face = make_sprite("assets/face01.png");
  bi_node_set_position(face,180,180);
  // Shader : Multiple Render Targets
  BiShaderNode *shader_node = bi_shader_node_init(ALLOC(BiShaderNode));
  shader_node->shader =  create_shader_with_default_vertex_shader("assets/shaders/mrt.frag");
  bi_node_add_node(shader_node, face);
  shader_node->textures[0] = bi_node_get_texture(face);
  bi_node_add_node(canvas, shader_node);
  // Draw
  bi_framebuffer_clear(canvas->framebuffer,0,0,0xff,0x80);
  bi_draw_framebuffer_node(context,canvas);
  return canvas;
}

int main(int argc, char* argv[])
{
  BiContext* context = make_context(__FILE__);

  // Framebuffer Canvas
  BiNode* canvas = draw_canvas(context);

  // 2 Texture from 1 Canvas
  BiNode* sprites[2];
  BiTexture* texs[2];
  for(int i=0;i<2;i++){
    sprites[i] = bi_node_init(ALLOC(BiNode));
    bi_node_set_size(sprites[i], canvas->w,canvas->h);
    bi_node_set_position(sprites[i], 10+i*220,10);
    sprites[i]->texture_flip_vertical = true; // UPSIDE DOWN
    texs[i] = bi_texture_init_with_texture_id(ALLOC(BiTexture),
                                              canvas->framebuffer->w, canvas->framebuffer->h,
                                              canvas->framebuffer->texture_ids[i] );
    bi_node_set_texture(sprites[i],texs[i],0,0,texs[i]->w,texs[i]->h);
  }

  // Main Layer
  BiShaderNode *shader_node = bi_shader_node_init(ALLOC(BiShaderNode));
  BiNode* background = bi_node_add_node(shader_node,make_bg("assets/check.png"));
  bi_node_add_node(shader_node, sprites[0]);
  bi_node_add_node(shader_node, sprites[1]);
  shader_node->textures[0] = bi_node_get_texture(background);
  shader_node->textures[1] = texs[0];
  shader_node->textures[2] = texs[1];
  bi_node_add_node(&context->default_framebuffer_node,shader_node);

  // start
  bi_start_run_loop(context);
  return 0;
}
