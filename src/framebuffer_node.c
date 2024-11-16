#include <bi/shader_node.h>
#include <bi/node.h>
#include <bi/render.h>
#include <bi/context.h>

//
// LayerGroup Draw
//
void bi_framebuffer_node_clear(BiNode* canvas,uint8_t r,uint8_t g,uint8_t b,uint8_t a)
{
  glBindFramebuffer(GL_FRAMEBUFFER, canvas->framebuffer->framebuffer_id);
  glClearColor(r/255.0, g/255.0, b/255.0, a/255.0);
  glClear(GL_COLOR_BUFFER_BIT);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void bi_framebuffer_node_draw(BiNode* canvas, BiContext* context)
{
  // viewport setting
  GLint tmp[4];
  glGetIntegerv(GL_VIEWPORT,tmp);
  glViewport(0,0,canvas->w,canvas->h);
  // camera
  int _w = context->w;
  int _h = context->h;
  context->w = canvas->w;
  context->h = canvas->h;
  // rendering
  BiRenderingContext rendering_context;
  bi_rendering_context_init(&rendering_context,true,true,1.0,
                            NULL, //&context->interaction_queue,
                            NULL, // &context->timer_queue,
                            &context->rendering_queue);
  bi_render_framebuffer_node( context, canvas, rendering_context );
  // restore
  context->w = _w;
  context->h = _h;
  // restore viewport
  glViewport(tmp[0],tmp[1],tmp[2],tmp[3]);
}
