#include <math.h>
#include <bi/render.h>
#include <bi/node.h>
#include <bi/context.h>
#include <bi/camera.h>
#include <bi/shader_node.h>
#include <bi/bi_gl.h>
#include <bi/framebuffer.h>
#include "matrix/matrix.h"

static inline void bi_render_shader_node(BiRenderingContext, BiShaderNode*);

void bi_render_node(BiRenderingContext rc, BiNode* n)
{
  // inherit
  rc._interaction_enabled = rc._interaction_enabled && n->interaction_enabled;
  n->timers._final_time_scale = rc._time_scale * n->time_scale;
  n->_final_visibility = n->visible && rc._visible;;
  rc._visible = n->_final_visibility;
  // Queueing
  if( rc.interaction_queue && rc._interaction_enabled && bi_node_is_event_handler_available(n) ) {
    array_add_object(rc.interaction_queue, n);
  }
  if( rc.timer_queue && n->timers.size > 0 ) {
    array_add_object(rc.timer_queue, n);
  }
  if( rc._rendering_queue && n->_final_visibility && n->w!=0 && n->h!=0 ) { // skip invisible node
    array_add_object(rc._rendering_queue,n);
  }
  // Children
  bool matrix_update_require = bi_node_update_matrix(n);
  array_sort(&n->children);
  for( int i=0; i<n->children.size; i++ ){
    BiNodeBase* tmp = bi_node_child_at(n,i);
    if(tmp->class == BI_NODE) {
      BiNode* child = bi_node_p( tmp );
      if( child->transform_matrix_cached == true && matrix_update_require ) {
        child->transform_matrix_cached = false;
      }
      bi_render_node(rc,child);
    }else if(tmp->class == BI_SHADER_NODE) {
      BiShaderNode* child = (BiShaderNode*)tmp;
      bi_render_shader_node(rc,child);
    }
  }
}

static inline void bi_render_shader_node(BiRenderingContext rc, BiShaderNode* snode)
{
  // invalid parent
  BiNode* parent = bi_node_p(snode->parent);
  if( parent == NULL || parent->framebuffer == NULL) {
    return;
  }
  BiFramebuffer *fb = parent->framebuffer;
  // Queueing Timer
  rc._time_scale *= snode->time_scale;
  if( rc.timer_queue && snode->timers.size > 0 ) {
    array_add_object(rc.timer_queue, snode);
  }
  // New Queue
  Array rendering_queue;
  Array *original_queue = rc._rendering_queue;
  rc._rendering_queue = &rendering_queue;
  array_init(rc._rendering_queue);
  // Queueing children nodes
  array_sort(&snode->children);
  for(int i=0;i<snode->children.size;i++){
    BiNode* child  = bi_node_p(array_object_at(&snode->children,i));
    // Clear Framebuffer
    if(child->framebuffer) bi_framebuffer_clear(child->framebuffer,0,0,0,0);
    bi_render_node(rc,child);
  }
  rc._rendering_queue = original_queue;
  if(rendering_queue.size==0) {
    array_clear(&rendering_queue);
    return;
  }

  // Set Target Framebuffer
  glBindFramebuffer(GL_FRAMEBUFFER, fb->framebuffer_id);
  if(fb->texture_num>0){
    GLenum list[BI_FRAMEBUFFER_TEXTURE_MAX];
    for(int i=0;i<fb->texture_num;i++){ list[i] = GL_COLOR_ATTACHMENT0+i; }
    glDrawBuffers(fb->texture_num,list);
  }
  // Set Viewport
  glViewport(0,0,fb->viewport_w,fb->viewport_h);
  // shader select
  BiShader* shader = snode->shader ? snode->shader : rc.default_shader;
  glUseProgram(shader->program_id);
  // uniforms
  double time = rc.time/1000.0;
  bi_shader_set_uniforms(shader,time,fb,snode);
  // Activate Textures
  for(int i=0;i<BI_SHADER_MAX_TEXTURES;i++) {
    glActiveTexture(GL_TEXTURE0+i);
    if( snode->textures[i] != NULL ) {
      snode->textures[i]->_texture_unit = i;
      glBindTexture(GL_TEXTURE_2D, snode->textures[i]->texture_id);
    }else{
      glBindTexture(GL_TEXTURE_2D, 0);
    }
  }
  // set projection and view
  GLfloat camera[16];
  bi_camera_matrix(camera,snode->camera_x,snode->camera_y,fb->w,fb->h,false);
  glUniformMatrix4fv(shader->uniform.camera, 1, GL_FALSE, camera);
  // blend function
  glBlendFuncSeparate(
    snode->blend_factor.src,
    snode->blend_factor.dst,
    snode->blend_factor.alpha_src,
    snode->blend_factor.alpha_dst
  );
  // Draw
  bi_shader_draw(shader,&rendering_queue);
  glFlush();
  // Unbind
  glBindFramebuffer(GL_FRAMEBUFFER,0);
  // Clean Queue
  array_clear(&rendering_queue);
}

BiRenderingContext* bi_rendering_context_init(BiRenderingContext* rendering_context,
                                              int64_t time,
                                              BiShader* default_shader,
                                              Array* interaction_queue,
                                              Array* timer_queue )
{
  rendering_context->_visible = true;
  rendering_context->_interaction_enabled = true;
  rendering_context->_time_scale = 1.0;
  rendering_context->time = time;
  rendering_context->default_shader = default_shader;
  rendering_context->interaction_queue = interaction_queue;
  rendering_context->timer_queue = timer_queue;
  rendering_context->_rendering_queue = NULL;
  return rendering_context;
}
