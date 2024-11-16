#include <math.h>
#include <bi/render.h>
#include <bi/node.h>
#include <bi/context.h>
#include <bi/camera.h>
#include <bi/shader_node.h>
#include <bi/bi_gl.h>
#include <bi/framebuffer.h>
#include "matrix/matrix.h"

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

static inline void bi_render_queuing_node(BiContext* context, BiRenderingContext rc, BiNode* n)
{
  bool visible = rc.visible;
  bool interaction_enabled = rc.interaction_enabled && n->interaction_enabled;

  rc.interaction_enabled = interaction_enabled;
  n->timers.scale = rc.time_scale * n->time_scale;
  n->final_visibility = n->visible && visible;

  // event handler and timer
  if( rc.interaction_queue && interaction_enabled && node_has_event_handler(n) ) {
    array_add_object(rc.interaction_queue, n);
  }
  if( rc.timer_queue && n->timers.size > 0 ) {
    array_add_object(rc.timer_queue, n);
  }
  // nodes ; skip invisible, zero-size and transparent
  if( visible==true && n->visible==true && n->w!=0 && n->h!=0 ) {
    array_add_object(rc.rendering_queue,n);
  }
  //
  bool matrix_update_require = bi_node_update_matrix(n);
  //
  rc.visible = visible && n->visible;
  // sort
  array_sort(&n->children);
  // queueing
  for( int i=0; i<n->children.size; i++ ){
    BiNode* tmp = bi_node_child_at(n,i);
    if( tmp->transform_matrix_cached == true && matrix_update_require ) {
      tmp->transform_matrix_cached = false;
    }
    bi_render_queuing_node(context, rc, tmp);
  }
}

static inline void render_shader_node_to_buffer(BiContext* context,
                                                BiShaderNode* shader_node,
                                                BiFramebuffer* fb,
                                                BiRenderingContext rc)
{
  // timer
  rc.time_scale *= shader_node->time_scale;
  if( rc.timer_queue && shader_node->timers.size > 0 ) {
    array_add_object(rc.timer_queue, shader_node);
  }
  // Queueing children nodes
  array_clear(rc.rendering_queue);
  array_sort(&shader_node->children);
  for(int i=0;i<shader_node->children.size;i++){
    bi_render_queuing_node(context, rc, (BiNode*)array_object_at(&shader_node->children,i) );
  }
  if(rc.rendering_queue->size==0) return;
  // shader select
  BiShader* shader = shader_node->shader ? shader_node->shader : &context->default_shader;
  glUseProgram(shader->program_id);
  // uniforms
  double time = (context->program_start_at - context->frame_start_at)/1000.0;
  int drawable_w,drawable_h;
  SDL_GL_GetDrawableSize(context->_window, &drawable_w, &drawable_h);
  float scale = (float)drawable_h / context->h;
  bi_shader_set_uniforms(shader,time,context->w,context->h,scale,shader_node->shader_extra_data);
  // Activate Textures
  for(int i=0;i<BI_SHADER_MAX_TEXTURES;i++) {
    glActiveTexture(GL_TEXTURE0+i);
    if( shader_node->textures[i] != NULL ) {
      shader_node->textures[i]->texture_unit = i;
      glBindTexture(GL_TEXTURE_2D, shader_node->textures[i]->texture_id);
    }else{
      glBindTexture(GL_TEXTURE_2D, 0);
    }
  }
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
  // Target
  glBindFramebuffer(GL_FRAMEBUFFER, fb->framebuffer_id);
  // Draw
  bi_shader_draw(shader,rc.rendering_queue);
  // Unbind
  glBindFramebuffer(GL_FRAMEBUFFER,0);
}


void bi_render_framebuffer_node(BiContext* context,
                                BiFramebufferNode *fbnode,
                                BiRenderingContext rc )
{
  // context
  rc.interaction_enabled = rc.interaction_enabled && fbnode->interaction_enabled;
  rc.time_scale *= fbnode->time_scale;
  // timer
  if( fbnode->timers.size > 0 ) {
    array_add_object(&context->timer_queue, fbnode);
  }
  // Clear
  glBindFramebuffer(GL_FRAMEBUFFER, fbnode->framebuffer.framebuffer_id);
  glClearColor(0,0,0,0);
  glClear(GL_COLOR_BUFFER_BIT);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  // Children
  array_sort(&fbnode->children);
  for( int i=0; i<fbnode->children.size; i++ ) {
    BiShaderNode* shader_node = array_object_at(&fbnode->children, i);
    render_shader_node_to_buffer( context, shader_node, &fbnode->framebuffer, rc );
  }
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
