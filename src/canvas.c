#include <bi/canvas.h>
#include <bi/camera.h>
#include <bi/bi_gl.h>
#include <bi/render.h>
#include <bi/node.h>
#include <bi/shader.h>
#include <bi/context.h>
#include "matrix/matrix.h"

BiCanvas* bi_canvas_init(BiCanvas* canvas,int w,int h)
{
  canvas->w = w;
  canvas->h = h;
  bi_framebuffer_init(&canvas->framebuffer,w,h);
  canvas->blend_factor = BI_BLEND_FACTOR_DEFAULT;
  canvas->shader = NULL;
  for(int i=0;i<4;i++) {
    canvas->shader_attributes[i] = 0;
  }
  for(int i=0;i<BI_LAYER_MAX_TEXTURES;i++) {
    canvas->textures[i] = NULL;
  }
  return canvas;
}

BiCanvas* bi_canvas_init_with_framebuffer(BiCanvas* canvas,BiFramebuffer *src)
{
  bi_canvas_init(canvas,src->w,src->h);
  glBindFramebuffer(GL_FRAMEBUFFER, canvas->framebuffer.framebuffer_id);
  glClearColor(0,0,0,0);
  glClear(GL_COLOR_BUFFER_BIT);
  glBindFramebuffer(GL_READ_FRAMEBUFFER, src->framebuffer_id);
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, canvas->framebuffer.framebuffer_id);
  glBlitFramebuffer(0, 0, src->w, src->h,
                    0, 0, src->w, src->h,
                    GL_COLOR_BUFFER_BIT, GL_NEAREST);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  return canvas;
}

void bi_canvas_draw(BiCanvas* canvas,BiNode* node)
{
  //
  if(node==NULL) return;
  bi_node_remove_from_parent(node);
  // queue
  Array queue;
  array_init(&queue);
  BiRenderingContext context;
  bi_rendering_context_init(&context,true,false,0,NULL,NULL,&queue);
  bi_render_queuing(context, node);
  if(queue.size==0) return;
  // viewport setting
  GLint original_viewport[4];
  glGetIntegerv(GL_VIEWPORT,original_viewport);
  glViewport(0,0,canvas->w,canvas->h);
  // bind
  glBindFramebuffer(GL_FRAMEBUFFER, canvas->framebuffer.framebuffer_id);
  // set shader
  BiShader* shader = canvas->shader;
  glUseProgram(shader->program_id);
  // uniforms
  bi_shader_set_uniforms(shader,0,canvas->w,canvas->h,1.0,canvas->shader_attributes);
  // Textures
  bi_render_activate_textures(canvas->textures);
  // set projection and view
  GLfloat camera[16];
  bi_camera_matrix(camera,0,0,canvas->w,canvas->h,true);
  glUniformMatrix4fv(shader->uniform.camera, 1, GL_FALSE, camera);
  // blend function
  glBlendFuncSeparate(
    canvas->blend_factor.src,
    canvas->blend_factor.dst,
    canvas->blend_factor.alpha_src,
    canvas->blend_factor.alpha_dst
  );
  // draw
  bi_shader_draw(shader,&queue);
  // unbind
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  // restore viewport
  glViewport(0,0,original_viewport[2],original_viewport[3]);
  // free queue
  array_clear(&queue);
}

void bi_canvas_clear(BiCanvas* canvas,uint8_t r,uint8_t g,uint8_t b,uint8_t a)
{
  glBindFramebuffer(GL_FRAMEBUFFER, canvas->framebuffer.framebuffer_id);
  glClearColor(r/255.0, g/255.0, b/255.0, a/255.0);
  glClear(GL_COLOR_BUFFER_BIT);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

BiTexture* bi_canvas_to_texture(BiCanvas* canvas,BiTexture* texture)
{
  texture->w = canvas->w;
  texture->h = canvas->h;
  texture->texture_id = canvas->framebuffer.texture_id;
  texture->texture_unit = 0;
  return texture;
}

void bi_canvas_save_png_image(BiCanvas* canvas,const char *filename)
{
  uint8_t* pixels = malloc(8*canvas->w*canvas->h);
  glBindFramebuffer(GL_FRAMEBUFFER,canvas->framebuffer.framebuffer_id);
  glReadPixels(0,0,canvas->w,canvas->h,GL_RGBA,GL_UNSIGNED_BYTE,pixels);
  glBindFramebuffer(GL_FRAMEBUFFER,0);
  const int w = canvas->w;
  const int h = canvas->h;
  const int pitch = 4*w;
  SDL_Surface* s = SDL_CreateRGBSurfaceWithFormatFrom(pixels,w,h,32,pitch,SDL_PIXELFORMAT_RGBA32);
  IMG_SavePNG(s,filename);
  SDL_FreeSurface(s);
  free(pixels);
}
