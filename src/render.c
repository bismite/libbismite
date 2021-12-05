#include <bi/render.h>
#include <bi/node.h>
#include <bi/context.h>
#include <bi/matrix.h>
#include <bi/layer.h>
#include <bi/bi_gl.h>
#include <math.h>

// context
typedef struct _RenderingContext{
  bool visible;
  bool interaction_enabled;
  double time_scale;
} RenderingContext;

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

void bi_render_update_matrix(BiNode* n)
{
  GLfloat tx = n->x;
  GLfloat ty = n->y;
  GLfloat sx = n->scale_x ;
  GLfloat sy = n->scale_y ;
  // XXX: non-zero angle calculation is super slow!
  GLfloat cos_v = cos(n->angle);
  GLfloat sin_v = sin(n->angle);
  // local
  GLfloat lx = - n->anchor_x * n->w;
  GLfloat ly = - n->anchor_y * n->h;
  GLfloat lsx = n->w;
  GLfloat lsy = n->h;

  // matrix chain
  if(n->parent == NULL){
    bi_mat4_identity( n->transform );
  }else{
    bi_mat4_copy( n->transform, n->parent->transform );
  }

  GLfloat trans[16] = {
    1.0, 0.0, 0.0, 0.0,
    0.0, 1.0, 0.0, 0.0,
    0.0, 0.0, 1.0, 0.0,
     tx,  ty, 0.0, 1.0
  };

  // cos -sin  0  0      cos  sin  0  0
  // sin  cos  0  0     -sin  cos  0  0
  //   0    0  1  0  =>    0    0  1  0
  //   0    0  0  1        0    0  0  1
  GLfloat rotation[16] = {
     cos_v, sin_v, 0.0, 0.0,
    -sin_v, cos_v, 0.0, 0.0,
       0.0,   0.0, 1.0, 0.0,
       0.0,   0.0, 0.0, 1.0
  };

  GLfloat scale[16] = {
   sx,  0,  0,  0,
    0, sy,  0,  0,
    0,  0,  1,  0,
    0,  0,  0,  1
  };

  GLfloat local_trans[16] = {
    1,  0,  0,  0,
    0,  1,  0,  0,
    0,  0,  1,  0,
   lx, ly,  0,  1
  };

  GLfloat local_scale[16] = {
   lsx,   0,  0,  0,
     0, lsy,  0,  0,
     0,   0,  1,  0,
     0,   0,  0,  1
  };

  bi_mat4_multiply(trans,n->transform,n->transform);
  bi_mat4_multiply(rotation,n->transform,n->transform);
  bi_mat4_multiply(scale,n->transform,n->transform);
  if( n->matrix_include_anchor_translate ) {
    bi_mat4_multiply(local_trans,n->transform,n->transform); // include local transform
    bi_mat4_multiply(local_scale,n->transform,n->draw);
  } else {
    bi_mat4_multiply(local_trans,n->transform,n->draw); // exclude local transform
    bi_mat4_multiply(local_scale,n->draw,n->draw);
  }
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

static void draw(BiContext* context, BiNode* n, RenderingContext render_context)
{
  bool visible = render_context.visible;
  bool interaction_enabled = render_context.interaction_enabled && n->interaction_enabled;

  render_context.interaction_enabled = interaction_enabled;
  n->timers.scale = render_context.time_scale * n->time_scale;
  n->_final_visibility = n->visible && visible;

  // event handler and timer
  if( interaction_enabled && node_has_event_handler(n) ) {
    array_add_object(&context->_interaction_queue, n);
  }
  if( n->timers.size > 0 ) {
    array_add_object(&context->_timer_queue, n);
  }

  // skip: invisible, zero-size node, transparent node
  if( visible==true && n->visible==true && n->w!=0 && n->h!=0 && n->opacity>0 ) {
    array_add_object(&context->_rendering_queue,n);
  }

  //
  bool matrix_update_require = false;
  if( n->matrix_cached == false ) {
      bi_render_update_matrix(n);
      n->matrix_cached = true;
      matrix_update_require = true;
      context->profile.matrix_updated += 1;
  }

  //
  render_context.visible = visible && n->visible;
  bi_render_node_sort(n);
  for( int i=0; i<n->children.size; i++ ){
    if( bi_node_child_at(n,i)->matrix_cached == true && matrix_update_require ) {
        bi_node_child_at(n,i)->matrix_cached = false;
    }
    draw(context, bi_node_child_at(n,i), render_context);
  }
}

static void render_layer(BiContext* context,BiLayer* layer, RenderingContext render_context)
{
  if( layer->root == NULL ) {
    return;
  }

  render_context.time_scale *= layer->time_scale;

  // timer
  if( layer->timers.size > 0 ) {
    array_add_object(&context->_timer_queue, layer);
  }

  // reset rendering queue
  array_clear(&context->_rendering_queue);

  //
  // recursive visit nodes
  //
  draw(context, layer->root, render_context);

  const size_t len = context->_rendering_queue.size;
  if(len==0) return;

  context->profile.rendering_nodes_queue_size += len;

  // shader select
  BiShader* shader = &context->default_shader;
  if( layer->shader != NULL ){
    shader = layer->shader;
  }
  glUseProgram(shader->program_id);

  // time
  glUniform1f(shader->time_location, (context->program_start_at - context->_frame_start_at)/1000.0 );
  // resolution
  glUniform2f(shader->resolution_location, context->w, context->h );
  // scale
  int drawable_w,drawable_h;
  SDL_GL_GetDrawableSize(context->window, &drawable_w, &drawable_h);
  glUniform1f(shader->scale_location, (float)drawable_h / context->h );
  // optional attributes
  glUniform4fv(shader->optional_attributes_location, 1, layer->shader_attributes );

  // Textures
  for(int i=0;i<BI_LAYER_MAX_TEXTURES;i++) {
    // texture bind
    glActiveTexture(GL_TEXTURE0+i);
    if( layer->textures[i] == NULL ) {
      glBindTexture(GL_TEXTURE_2D, context->default_texture);
    }else{
      layer->textures[i]->_texture_unit = i;
      glBindTexture(GL_TEXTURE_2D, layer->textures[i]->texture_id);
    }
  }

  //
  // copy to buffer
  //
  GLfloat uv[4*len]; // [ left, top, right, bottom ]
  GLfloat texture_z[len];
  GLfloat opacity[len];
  GLfloat transforms[len][16];
  GLfloat tint[len][4];
  for(int i=0;i<len;i++){
    BiNode* node = context->_rendering_queue.objects[i];
    opacity[i] = node->opacity;
    if(node->texture_mapping != NULL) {
      // Left-Top is 0-0, Right-Bottom is 1-1.
      BiTextureMapping *t = node->texture_mapping;
      uv[i*4+0] = t->boundary[0]; // Left
      uv[i*4+1] = t->boundary[1]; // Top
      uv[i*4+2] = t->boundary[2]; // Right
      uv[i*4+3] = t->boundary[3]; // Bottom
      if(t->flip_horizontal) {
        uv[i*4+0] = t->boundary[2];
        uv[i*4+2] = t->boundary[0];
      }
      if(t->flip_vertical) {
        uv[i*4+1] = t->boundary[3];
        uv[i*4+3] = t->boundary[1];
      }
      texture_z[i] = t->texture->_texture_unit; // texture
    }else{
      texture_z[i] = -1; // no-texture
    }

    //
    bi_mat4_copy(transforms[i], node->draw);

    // color
    tint[i][0] = node->color[0] / 255.0;
    tint[i][1] = node->color[1] / 255.0;
    tint[i][2] = node->color[2] / 255.0;
    tint[i][3] = node->color[3] / 255.0;
  }

  //
  // update vbo
  // orphaning: https://www.khronos.org/opengl/wiki/Buffer_Object_Streaming#Buffer_re-specification
  //
  // opacity
  glBindBuffer(GL_ARRAY_BUFFER, shader->opacity_buffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 1 * len, NULL, GL_DYNAMIC_DRAW);
  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(GLfloat) * 1 * len, opacity);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  // texture_z (texture index send as float. not integer.)
  glBindBuffer(GL_ARRAY_BUFFER, shader->texture_z_buffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 1 * len, NULL, GL_DYNAMIC_DRAW);
  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(GLfloat) * 1 * len, texture_z);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  // uv
  glBindBuffer(GL_ARRAY_BUFFER, shader->uv_buffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 4 * len, NULL, GL_DYNAMIC_DRAW);
  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(GLfloat) * 4 * len, uv);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  // transform
  glBindBuffer(GL_ARRAY_BUFFER, shader->transform_buffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 16 * len, NULL, GL_DYNAMIC_DRAW);
  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(GLfloat) * 16 * len, transforms);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  // tint color
  glBindBuffer(GL_ARRAY_BUFFER, shader->tint_color_buffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 4 * len, NULL, GL_DYNAMIC_DRAW);
  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(GLfloat) * 4 * len, tint);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  // set projection and view
  bi_shader_set_camera(shader, context->w, context->h, layer->camera_x, layer->camera_y, false);

  // blend function
  glBlendFuncSeparate(
    layer->blend_factor.src,
    layer->blend_factor.dst,
    layer->blend_factor.alpha_src,
    layer->blend_factor.alpha_dst
  );

  //
  // Draw instances
  //
  glBindVertexArray(shader->vao);
    glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, len);
  glBindVertexArray(0);
}

static void render_texture(BiContext* context, GLuint texture, BiRawNode* target)
{
  BiTexture t;
  t.texture_id = texture;
  t.w = context->w;
  t.h = context->h;
  t._texture_unit = 0;

  BiTextureMapping m;
  bi_texture_mapping_init(&m,&t);
  m.flip_vertical = true;

  BiNode n;
  bi_node_init(&n);
  bi_node_set_size(&n,t.w,t.h);
  n.texture_mapping = &m;

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

  RenderingContext rcontext = {true,false};
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
                               RenderingContext rcontext
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
  RenderingContext rc = {true,true,1.0};
  render_layer_group(context,&context->layers,0,rc);

  //
  SDL_GL_SwapWindow(context->window);
}
