#include <math.h>
#include <bi/render.h>
#include <bi/node.h>
#include <bi/context.h>
#include <bi/camera.h>
#include <bi/layer.h>
#include <bi/bi_gl.h>
#include "matrix/matrix.h"

typedef bool (*render_function)(BiContext*, BiLayerBase*, BiFramebuffer*, BiRenderingContext);

static int node_order_compare(const void *_a, const void *_b )
{
  const BiNode *a = *(BiNode**)_a;
  const BiNode *b = *(BiNode**)_b;
  return a->z == b->z ? a->index - b->index : a->z - b->z;
}

static void bi_render_node_sort(BiNode* node)
{
  if( node->children_order_cached == false ) {
    for( int i=0; i<node->children.size; i++ ){ bi_node_child_at(node,i)->index = i; }
    qsort( node->children.objects, node->children.size, sizeof(BiNode*), node_order_compare);
    node->children_order_cached = true;
  }
}

static int layer_order_compare(const void *_a, const void *_b )
{
  const BiNodeBase *a = *(BiNodeBase**)_a;
  const BiNodeBase *b = *(BiNodeBase**)_b;
  return a->z == b->z ? a->index - b->index : a->z - b->z;
}

static void layer_sort(BiLayerGroup* layer_group)
{
  Array* a = &layer_group->layers;
  for( int i=0; i<a->size; i++ ){ ((BiNodeBase*)(a->objects[i]))->index = i; }
  qsort( a->objects, a->size, sizeof(BiNodeBase*), layer_order_compare);
}

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

void bi_render_queuing(BiRenderingContext context, BiNode* n)
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

  // skip: invisible, zero-size node, transparent node
  if( visible==true && n->visible==true && n->w!=0 && n->h!=0 ) {
    array_add_object(context.rendering_queue,n);
  }

  //
  bool matrix_update_require = bi_node_update_matrix(n);

  //
  context.visible = visible && n->visible;
  bi_render_node_sort(n);
  for( int i=0; i<n->children.size; i++ ){
    if( bi_node_child_at(n,i)->transform_matrix_cached == true && matrix_update_require ) {
      bi_node_child_at(n,i)->transform_matrix_cached = false;
    }
    bi_render_queuing(context, bi_node_child_at(n,i));
  }
}

void bi_render_activate_textures(BiTexture* textures[BI_LAYER_MAX_TEXTURES])
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

static void draw_layer_to_buffer(BiContext* context, BiLayer* layer, BiRenderingContext* rendering_context)
{
  if( layer->root == NULL ) return;

  BiRenderingContext rc = *rendering_context;

  rc.time_scale *= layer->time_scale;

  // timer
  if( rc.timer_queue && layer->timers.size > 0 ) {
    array_add_object(rc.timer_queue, layer);
  }

  // reset rendering queue
  array_clear(rc.rendering_queue);

  // recursive visit nodes
  bi_render_queuing(rc, layer->root);

  if(rc.rendering_queue->size==0) return;

  // context->profile.rendering_nodes_queue_size += len;

  // shader select
  BiShader* shader = &context->default_shader;
  if( layer->shader != NULL ){
    shader = layer->shader;
  }
  glUseProgram(shader->program_id);

  // uniforms
  double time = (context->program_start_at - context->frame_start_at)/1000.0;
  int drawable_w,drawable_h;
  SDL_GL_GetDrawableSize(context->window, &drawable_w, &drawable_h);
  float scale = (float)drawable_h / context->h;
  bi_shader_set_uniforms(shader,time,context->w,context->h,scale,layer->shader_extra_data);

  // textures
  bi_render_activate_textures(layer->textures);

  // set projection and view
  GLfloat camera[16];
  bi_camera_matrix(camera,layer->camera_x,layer->camera_y,context->w,context->h,false);
  glUniformMatrix4fv(shader->uniform.camera, 1, GL_FALSE, camera);

  // blend function
  glBlendFuncSeparate(
    layer->blend_factor.src,
    layer->blend_factor.dst,
    layer->blend_factor.alpha_src,
    layer->blend_factor.alpha_dst
  );

  bi_shader_draw(shader,rc.rendering_queue);
}

static void target_and_clear_framebuffer(BiFramebuffer *fb)
{
  glBindFramebuffer(GL_FRAMEBUFFER, fb->framebuffer_id);
  glClearColor(0,0,0,0);
  glClear(GL_COLOR_BUFFER_BIT);
}

extern void render_postprocess(BiContext* context,
                               BiLayerBase *layer,
                               BiFramebuffer *dst,
                               BiRenderingContext rc
                              )
{
  BiLayer *l = (BiLayer*)layer;
  // render to Temporary Framebuffer
  target_and_clear_framebuffer(&context->post_process_framebuffer);
  draw_layer_to_buffer( context, l, &rc );
  // Blit temporary framebuffer to fb
  glBindFramebuffer(GL_READ_FRAMEBUFFER, context->post_process_framebuffer.framebuffer_id);
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, dst->framebuffer_id);
  glBlitFramebuffer(0, 0, dst->w, dst->h,
                    0, 0, dst->w, dst->h,
                    GL_COLOR_BUFFER_BIT, GL_NEAREST);
  // back
  glBindFramebuffer(GL_FRAMEBUFFER, dst->framebuffer_id);
}

extern void render_layer(BiContext* context,
                         BiLayerBase *layer,
                         BiFramebuffer *fb,
                         BiRenderingContext rc
                        )
{
  draw_layer_to_buffer( context, (BiLayer*)layer, &rc );
}

extern void render_layer_group(BiContext* context,
                               BiLayerBase *layer_group,
                               BiFramebuffer* dst,
                               BiRenderingContext rc
                              )
{
  BiLayerGroup *lg = (BiLayerGroup*)layer_group;
  // context
  rc.interaction_enabled = rc.interaction_enabled && lg->interaction_enabled;
  rc.time_scale *= lg->time_scale;
  // timer
  if( lg->timers.size > 0 ) {
    array_add_object(&context->timer_queue, lg);
  }
  // render
  target_and_clear_framebuffer(&lg->framebuffer);
  layer_sort(lg);
  for( int i=0; i<lg->layers.size; i++ ) {
    BiLayerBase* l = lg->layers.objects[i];
    render_function f = l->_render_function_;
    f( context, l, &lg->framebuffer, rc );
  }
  // draw LayerGroup
  GLuint dst_id = dst ? dst->framebuffer_id : 0;
  BiFramebuffer *src = &lg->framebuffer;
  glBindFramebuffer(GL_FRAMEBUFFER, dst_id);
  BiTexture t;
  bi_texture_init_with_framebuffer(&t,src);
  BiNode n;
  bi_node_init(&n);
  bi_node_set_size(&n,context->w,context->h);
  bi_node_set_texture(&n, &t, 0,0,t.w,t.h);
  n.texture_flip_vertical = true;
  BiLayer l;
  bi_layer_init(&l);
  l.root = &n;
  l.textures[0] = &t;
  BiRenderingContext rcontext;
  bi_rendering_context_init(&rcontext,true,false,0,
                            NULL,
                            NULL,
                            &context->rendering_queue);
  draw_layer_to_buffer(context,&l,&rcontext);
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

  render_function f = context->layers._render_function_;
  f(context,(BiLayerBase*)&context->layers,NULL,rendering_context);

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
