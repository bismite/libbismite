#include <bi/render.h>
#include <bi/node.h>
#include <bi/context.h>
#include <bi/matrix.h>
#include <bi/camera.h>
#include <bi/layer.h>
#include <bi/bi_gl.h>
#include <math.h>

static int node_order_compare(const void *_a, const void *_b )
{
  const BiNode *a = *(BiNode**)_a;
  const BiNode *b = *(BiNode**)_b;
  return a->z == b->z ? a->_index - b->_index : a->z - b->z;
}

void bi_render_node_sort(BiNode* node)
{
  if( node->children_order_cached == false ) {
    for( int i=0; i<node->children.size; i++ ){ bi_node_child_at(node,i)->_index = i; }
    qsort( node->children.objects, node->children.size, sizeof(BiNode*), node_order_compare);
    node->children_order_cached = true;
  }
}

static int layer_order_compare(const void *_a, const void *_b )
{
  const BiRawNode *a = *(BiRawNode**)_a;
  const BiRawNode *b = *(BiRawNode**)_b;
  return a->z == b->z ? a->_index - b->_index : a->z - b->z;
}

static void layer_sort(BiLayerGroup* layer_group)
{
  Array* a = &layer_group->layers;
  for( int i=0; i<a->size; i++ ){ ((BiRawNode*)(a->objects[i]))->_index = i; }
  qsort( a->objects, a->size, sizeof(BiRawNode*), layer_order_compare);
}

static bool node_has_event_handler(BiNode* n)
{
  if(n->_on_move_cursor != NULL ||
     n->_on_click != NULL ||
     n->_on_move_finger != NULL ||
     n->_on_keyinput != NULL ||
     n->_on_touch != NULL ||
     n->_on_textinput != NULL
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
  n->_final_visibility = n->visible && visible;

  // event handler and timer
  if( context.interaction_queue && interaction_enabled && node_has_event_handler(n) ) {
    array_add_object(context.interaction_queue, n);
  }
  if( context.timer_queue && n->timers.size > 0 ) {
    array_add_object(context.timer_queue, n);
  }

  // skip: invisible, zero-size node, transparent node
  if( visible==true && n->visible==true && n->_w!=0 && n->_h!=0 && n->opacity>0 ) {
    array_add_object(context.rendering_queue,n);
  }

  //
  bool matrix_update_require = bi_node_update_matrix(n);

  //
  context.visible = visible && n->visible;
  bi_render_node_sort(n);
  for( int i=0; i<n->children.size; i++ ){
    if( bi_node_child_at(n,i)->matrix_cached == true && matrix_update_require ) {
      bi_node_child_at(n,i)->matrix_cached = false;
    }
    bi_render_queuing(context, bi_node_child_at(n,i));
  }
}

void bi_render_activate_textures(BiTexture** textures)
{
  // Textures
  for(int i=0;i<BI_LAYER_MAX_TEXTURES;i++) {
    glActiveTexture(GL_TEXTURE0+i);
    if( textures[i] == NULL ) {
      glBindTexture(GL_TEXTURE_2D, 0);
    }else{
      textures[i]->_texture_unit = i;
      glBindTexture(GL_TEXTURE_2D, textures[i]->texture_id);
    }
  }
}

static void render_layer(BiContext* context,BiLayer* layer, BiRenderingContext render_context)
{
  if( layer->root == NULL ) {
    return;
  }

  render_context.time_scale *= layer->time_scale;

  // timer
  if( render_context.timer_queue && layer->timers.size > 0 ) {
    array_add_object(render_context.timer_queue, layer);
  }

  // reset rendering queue
  array_clear(render_context.rendering_queue);

  // recursive visit nodes
  bi_render_queuing(render_context, layer->root);

  if(render_context.rendering_queue->size==0) return;

  // context->profile.rendering_nodes_queue_size += len;

  // shader select
  BiShader* shader = &context->default_shader;
  if( layer->shader != NULL ){
    shader = layer->shader;
  }
  glUseProgram(shader->program_id);

  // uniforms
  double time = (context->program_start_at - context->_frame_start_at)/1000.0;
  int drawable_w,drawable_h;
  SDL_GL_GetDrawableSize(context->window, &drawable_w, &drawable_h);
  float scale = (float)drawable_h / context->h;
  bi_shader_set_uniforms(shader,time,context->w,context->h,scale,layer->shader_attributes);

  // textures
  bi_render_activate_textures(layer->textures);

  // set projection and view
  GLfloat camera[16];
  bi_camera_matrix(camera,layer->camera_x,layer->camera_y,context->w,context->h,false);
  glUniformMatrix4fv(shader->camera_location, 1, GL_FALSE, camera);

  // blend function
  glBlendFuncSeparate(
    layer->blend_factor.src,
    layer->blend_factor.dst,
    layer->blend_factor.alpha_src,
    layer->blend_factor.alpha_dst
  );

  bi_shader_draw(shader,render_context.rendering_queue);
}

static void render_texture(BiContext* context, GLuint texture, BiRawNode* target)
{
  BiTexture t;
  t.texture_id = texture;
  t.w = context->w;
  t.h = context->h;
  t._texture_unit = 0;

  BiNode n;
  bi_node_init(&n);
  bi_node_set_size(&n,t.w,t.h);
  bi_node_set_texture(&n,&t,0,0,t.w,t.h);
  n._texture_flip_vertical = true;

  BiLayer l;
  bi_layer_init(&l);
  l.root = &n;
  l.textures[0] = &t;

  BiTexture t2;
  if(target){
    if(target->type == BI_NODE_TYPE_LAYER_GROUP) {
      BiLayerGroup *lg = (BiLayerGroup*)target;
      l.blend_factor = lg->blend_factor;
    }else if(target->type == BI_NODE_TYPE_LAYER) {
      BiLayer *layer = (BiLayer*)target;
      if(layer->post_process.framebuffer_enabled){
        t2.texture_id = context->_layer_framebuffer.texture_id;
        t2.w = context->w;
        t2.h = context->h;
        t2._texture_unit = 0;
        l.textures[1] = &t2;
      }
      memcpy(l.shader_attributes, layer->post_process.shader_attributes, sizeof(GLfloat)*4 );
      l.shader = layer->post_process.shader;
      l.blend_factor = layer->post_process.blend_factor;
    }
  }

  BiRenderingContext rcontext;
  bi_rendering_context_init(&rcontext,true,false,0,
                            NULL,
                            NULL,
                            &context->_rendering_queue);
  render_layer(context,&l,rcontext);
}

static void target_and_clear_framebuffer(GLuint framebuffer_id)
{
  glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_id);
  glClearColor(0,0,0,0);
  glClear(GL_COLOR_BUFFER_BIT);
}

static void render_layer_group(BiContext* context,
                               BiLayerGroup *lg,
                               GLuint parent_framebuffer_id,
                               BiRenderingContext rcontext
                              )
{
  rcontext.interaction_enabled = rcontext.interaction_enabled && lg->interaction_enabled;
  target_and_clear_framebuffer(lg->framebuffer.framebuffer_id);

  rcontext.time_scale *= lg->time_scale;

  // timer
  if( lg->timers.size > 0 ) {
    array_add_object(&context->_timer_queue, lg);
  }

  // render
  layer_sort(lg);
  for( int i=0; i<lg->layers.size; i++ ) {
    BiRawNode* n = lg->layers.objects[i];
    if( n->type == BI_NODE_TYPE_LAYER_GROUP ) {
      // Layer Group
      render_layer_group( context, (BiLayerGroup*)n, lg->framebuffer.framebuffer_id, rcontext );
    } else if( n->type == BI_NODE_TYPE_LAYER ) {
      // Layer
      BiLayer *layer = (BiLayer*)n;
      if(layer->post_process.framebuffer_enabled){
        // render to framebuffer
        target_and_clear_framebuffer(context->_layer_framebuffer.framebuffer_id);
      }
      render_layer( context, layer, rcontext );
      if(layer->post_process.shader){
        // target new framebuffer
        target_and_clear_framebuffer(context->_post_process_framebuffer.framebuffer_id);
        // render
        render_texture(context,lg->framebuffer.texture_id,n);
        // swap framebuffer
        BiFramebuffer tmp = lg->framebuffer;
        lg->framebuffer = context->_post_process_framebuffer;
        context->_post_process_framebuffer = tmp;
      }
      // back
      glBindFramebuffer(GL_FRAMEBUFFER, lg->framebuffer.framebuffer_id);
    }
  }

  // finalize
  glBindFramebuffer(GL_FRAMEBUFFER, parent_framebuffer_id);
  render_texture(context,lg->framebuffer.texture_id,(BiRawNode*)lg);
}

void bi_render(BiContext* context)
{
  // clear
  glClearColor(context->color[0]/255.f,
               context->color[1]/255.f,
               context->color[2]/255.f,
               context->color[3]/255.f );
  glClear(GL_COLOR_BUFFER_BIT);

  // reset stats
  context->profile.matrix_updated = 0;
  context->profile.rendering_nodes_queue_size = 0;

  // rendering
  BiRenderingContext rendering_context;
  bi_rendering_context_init(&rendering_context,true,true,1.0,
                            &context->_interaction_queue,
                            &context->_timer_queue,
                            &context->_rendering_queue);
  render_layer_group(context,&context->layers,0,rendering_context);

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
