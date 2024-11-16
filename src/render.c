#include <math.h>
#include <bi/render.h>
#include <bi/node.h>
#include <bi/context.h>
#include <bi/camera.h>
#include <bi/layer.h>
#include <bi/bi_gl.h>
#include "matrix/matrix.h"

typedef void (*render_function)(BiContext*, BiNodeBase*, BiFramebuffer*, BiRenderingContext);

static bool node_has_event_handler(BiNode* n)
{
  if(n->on_move_cursor != NULL ||
     n->on_click != NULL ||
     n->on_move_finger != NULL ||
     n->on_keyinput != NULL ||
     n->on_touch != NULL ||
     n->on_textinput != NULL
   ) {
    return true;
  }
  return false;
}

static inline void bi_render_queuing(BiRenderingContext context, BiNode* n)
{
  bool visible = context.visible;
  bool interaction_enabled = context.interaction_enabled && n->interaction_enabled;

  context.interaction_enabled = interaction_enabled;
  n->timers.scale = context.time_scale * n->time_scale;
  n->final_visibility = n->visible && visible;

  // event handler and timer
  if( context.interaction_queue && interaction_enabled && node_has_event_handler(n) ) {
    array_add_object(context.interaction_queue, n);
  }
  if( context.timer_queue && n->timers.size > 0 ) {
    array_add_object(context.timer_queue, n);
  }
  // nodes ; skip invisible, zero-size and transparent
  if( visible==true && n->visible==true && n->w!=0 && n->h!=0 ) {
    array_add_object(context.rendering_queue,n);
  }
  //
  bool matrix_update_require = bi_node_update_matrix(n);
  //
  context.visible = visible && n->visible;
  // sort and queueing
  array_sort(&n->children);
  for( int i=0; i<n->children.size; i++ ){
    if( bi_node_child_at(n,i)->transform_matrix_cached == true && matrix_update_require ) {
      bi_node_child_at(n,i)->transform_matrix_cached = false;
    }
    bi_render_queuing(context, bi_node_child_at(n,i));
  }
}

static inline void bi_render_activate_textures(BiTexture* textures[BI_LAYER_MAX_TEXTURES])
{
  for(int i=0;i<BI_LAYER_MAX_TEXTURES;i++) {
    glActiveTexture(GL_TEXTURE0+i);
    if( textures[i] != NULL ) {
      textures[i]->texture_unit = i;
      glBindTexture(GL_TEXTURE_2D, textures[i]->texture_id);
    }else{
      glBindTexture(GL_TEXTURE_2D, 0);
    }
  }
}

static void draw_shader_node_to_buffer(BiContext* context, BiShaderNode* shader_node, BiRenderingContext* rendering_context)
{
  BiRenderingContext rc = *rendering_context;
  // timer
  rc.time_scale *= shader_node->time_scale;
  if( rc.timer_queue && shader_node->timers.size > 0 ) {
    array_add_object(rc.timer_queue, shader_node);
  }
  // queue
  array_clear(rc.rendering_queue);
  array_sort(&shader_node->children);
  for(int i=0;i<shader_node->children.size;i++){
    bi_render_queuing(rc, (BiNode*)array_object_at(&shader_node->children,i) );
  }
  if(rc.rendering_queue->size==0) return;
  // shader select
  BiShader* shader = shader_node->shader ? shader_node->shader : &context->default_shader;
  glUseProgram(shader->program_id);
  // uniforms
  double time = (context->program_start_at - context->frame_start_at)/1000.0;
  int drawable_w,drawable_h;
  SDL_GL_GetDrawableSize(context->window, &drawable_w, &drawable_h);
  float scale = (float)drawable_h / context->h;
  bi_shader_set_uniforms(shader,time,context->w,context->h,scale,shader_node->shader_extra_data);
  // textures
  bi_render_activate_textures(shader_node->textures);
  // set projection and view
  GLfloat camera[16];
  bi_camera_matrix(camera,shader_node->camera_x,shader_node->camera_y,context->w,context->h,false);
  glUniformMatrix4fv(shader->uniform.camera, 1, GL_FALSE, camera);
  // blend function
  glBlendFuncSeparate(
    shader_node->blend_factor.src,
    shader_node->blend_factor.dst,
    shader_node->blend_factor.alpha_src,
    shader_node->blend_factor.alpha_dst
  );
  bi_shader_draw(shader,rc.rendering_queue);
}

extern void render_postprocess(BiContext* context,
                               BiNodeBase *shader_node,
                               BiFramebuffer *dst,
                               BiRenderingContext rc
                              )
{
  BiShaderNode *l = (BiShaderNode*)shader_node;
  // render to Temporary Framebuffer
  glBindFramebuffer(GL_FRAMEBUFFER, context->post_process_framebuffer.framebuffer_id);
  glClearColor(0,0,0,0);
  glClear(GL_COLOR_BUFFER_BIT);
  draw_shader_node_to_buffer( context, l, &rc );
  glBindFramebuffer(GL_FRAMEBUFFER,0);
  // Blit temporary framebuffer to Destination FB
  glBindFramebuffer(GL_READ_FRAMEBUFFER, context->post_process_framebuffer.framebuffer_id);
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, dst->framebuffer_id);
  glBlitFramebuffer(0, 0, dst->w, dst->h,
                    0, 0, dst->w, dst->h,
                    GL_COLOR_BUFFER_BIT, GL_NEAREST);
  glBindFramebuffer(GL_READ_FRAMEBUFFER,0);
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER,0);
}

extern void render_shader_node(BiContext* context,
                         BiNodeBase *shader_node,
                         BiFramebuffer *dst,
                         BiRenderingContext rc
                        )
{
  draw_shader_node_to_buffer( context, (BiShaderNode*)shader_node, &rc );
}

static void draw_framebuffer_node_to_buffer(BiContext* context, BiFramebufferNode *lg, GLuint dst)
{
  BiFramebuffer *src = &lg->framebuffer;
  BiTexture t;
  bi_texture_init_with_framebuffer(&t,src);
  BiNode n;
  bi_node_init(&n);
  bi_node_set_size( &n, context->w,context->h);
  bi_node_set_texture(&n, &t, 0,0,t.w,t.h);
  n.texture_flip_vertical = true;
  BiShaderNode l;
  bi_shader_node_init(&l);
  bi_shader_node_add_node(&l,&n);
  l.textures[0] = &t;
  BiRenderingContext rcontext;
  bi_rendering_context_init(&rcontext,true,false,0,
                            NULL,
                            NULL,
                            &context->rendering_queue);
  // bind
  glBindFramebuffer(GL_FRAMEBUFFER, dst);
  // draw
  draw_shader_node_to_buffer(context,&l,&rcontext);
  // dealloc node array
  bi_node_base_deinit((BiNodeBase*)&l);
  // unbind
  glBindFramebuffer(GL_FRAMEBUFFER,0);
}

extern void bi_render_framebuffer_node(BiContext* context,
                               BiNodeBase *framebuffer_node,
                               BiRenderingContext rc
                              )
{
  render_function f=NULL;
  BiFramebufferNode *lg = (BiFramebufferNode*)framebuffer_node;
  // context
  rc.interaction_enabled = rc.interaction_enabled && lg->interaction_enabled;
  rc.time_scale *= lg->time_scale;
  // timer
  if( lg->timers.size > 0 ) {
    array_add_object(&context->timer_queue, lg);
  }
  // Target
  glBindFramebuffer(GL_FRAMEBUFFER, lg->framebuffer.framebuffer_id);
  glClearColor(0,0,0,0);
  glClear(GL_COLOR_BUFFER_BIT);
  // Children
  array_sort(&lg->children);
  for( int i=0; i<lg->children.size; i++ ) {
    BiNodeBase* n = array_object_at(&lg->children, i);
    switch(n->class) {
    case BI_NODE:
      // nop
      break;
    case BI_LAYER:
      f = ((BiShaderNode*)n)->_render_function_;
      f( context, n, &lg->framebuffer, rc );
      break;
    case BI_LAYER_GROUP:
      // render Child Framebuffer
      bi_render_framebuffer_node(context,n,rc);
      // Draw
      draw_framebuffer_node_to_buffer(context, (BiFramebufferNode*)n, lg->framebuffer.framebuffer_id );
      // Re-Target
      glBindFramebuffer(GL_FRAMEBUFFER, lg->framebuffer.framebuffer_id);
      break;
    }
  }
  // Unbind
  glBindFramebuffer(GL_FRAMEBUFFER,0);
}

void bi_render(BiContext* context)
{
  // clear
  glClearColor(context->color.r/255.f,
               context->color.g/255.f,
               context->color.b/255.f,
               context->color.a/255.f );
  glClear(GL_COLOR_BUFFER_BIT);
  // reset stats
  context->profile.matrix_updated = 0;
  context->profile.rendering_nodes_queue_size = 0;
  // rendering
  BiRenderingContext rendering_context;
  bi_rendering_context_init(&rendering_context,true,true,1.0,
                            &context->interaction_queue,
                            &context->timer_queue,
                            &context->rendering_queue);
  bi_render_framebuffer_node( context, (BiNodeBase*)&context->shader_nodes, rendering_context );
  // Blit Framebuffer
  glBindFramebuffer(GL_READ_FRAMEBUFFER, context->shader_nodes.framebuffer.framebuffer_id);
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
  glBlitFramebuffer(0, 0, context->w, context->h,
                    0, 0, context->w, context->h,
                    GL_COLOR_BUFFER_BIT, GL_NEAREST);
  glBindFramebuffer(GL_READ_FRAMEBUFFER,0);
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER,0);

  //
  SDL_GL_SwapWindow(context->window);
}

BiRenderingContext* bi_rendering_context_init(BiRenderingContext* context,
                                              bool visible,
                                              bool interaction_enabled,
                                              double time_scale,
                                              Array* interaction_queue,
                                              Array* timer_queue,
                                              Array* rendering_queue)
{
  context->visible = visible;
  context->interaction_enabled = interaction_enabled;
  context->time_scale = time_scale;
  context->interaction_queue = interaction_queue;
  context->timer_queue = timer_queue;
  context->rendering_queue = rendering_queue;
  return context;
}
