#include <bi/render.h>
#include <bi/node.h>
#include <bi/context.h>
#include <bi/matrix.h>
#include <bi/layer.h>
#include <bi/bi_gl.h>
#include <math.h>

static void set_projection(BiShader* shader,int w, int h, bool centering)
{
    if(centering) {
      int l = -w/2;
      int r =  w/2;
      int t =  h/2;
      int b = -h/2;

      GLfloat projection[16] = {
        2.0/(r-l),           0.0, 0.0, 0.0,
               0.0,    2.0/(t-b), 0.0, 0.0,
               0.0,          0.0, 1.0, 0.0,
      -(r+l)/(r-l), -(t+b)/(t-b), 0.0, 1.0
      };

      glUniformMatrix4fv(shader->projection_location, 1, GL_FALSE, projection);

    }else{
      GLfloat projection[16] = {
        2.0/w,   0.0, 0.0, 0.0,
          0.0, 2.0/h, 0.0, 0.0,
          0.0,   0.0, 1.0, 0.0,
         -1.0,  -1.0, 0.0, 1.0
      };

      glUniformMatrix4fv(shader->projection_location, 1, GL_FALSE, projection);
    }
}

static void set_camera(BiShader* shader,int x, int y)
{
    GLfloat view[16] = {
      1.0, 0.0, 0.0, 0.0,
      0.0, 1.0, 0.0, 0.0,
      0.0, 0.0, 1.0, 0.0,
       -x,  -y, 0.0, 1.0
    };

    glUniformMatrix4fv(shader->view_location, 1, GL_FALSE, view);
}

static void update_matrix(BiNode* n)
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
  if(n->_on_update != NULL ||
     n->_on_move_cursor != NULL ||
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

static void draw(BiContext* context, BiNode* n, bool visible)
{
    n->_final_visibility = n->visible && visible;

    // add callback
    if( node_has_event_handler(n) || n->timers.size > 0 ) {
      context->callback_planned_nodes[context->callback_planned_nodes_size] = n;
      context->callback_planned_nodes_size += 1;
    }

    // skip: invisible, zero-size node, transparent node
    if( visible==true && n->visible==true && n->w!=0 && n->h!=0 && n->color[3]!=0) {
      context->rendering_nodes_queue[context->rendering_nodes_queue_size] = n;
      context->rendering_nodes_queue_size += 1;
    }

    //
    bool matrix_update_require = false;
    if( n->matrix_cached == false ) {
        update_matrix(n);
        n->matrix_cached = true;
        matrix_update_require = true;
        context->profile.matrix_updated += 1;
    }

    //
    bi_node_sort(n);
    for( int i=0; i<n->children.size; i++ ){
      if( bi_node_child_at(n,i)->matrix_cached == true && matrix_update_require ) {
          bi_node_child_at(n,i)->matrix_cached = false;
      }
      draw(context, bi_node_child_at(n,i), visible && n->visible ? true : false);
    }
}

static void render_layer(BiContext* context,BiLayer* layer)
{
    if( layer->root == NULL ) {
      return;
    }

    // reset rendering queue
    context->rendering_nodes_queue_size = 0;

    //
    // recursive visit nodes
    //
    draw(context, layer->root, true);

    const size_t len = context->rendering_nodes_queue_size;

    if(len==0) return; // eraly exit

    context->profile.rendering_nodes_queue_size += len;

    // shader select
    BiShader* shader = &context->default_shader;
    if( layer->shader != NULL ){
      shader = layer->shader;
    }
    glUseProgram(shader->program_id);

    // time
    glUniform1f(shader->time_location, (context->program_start_at - context->frame_start_at)/1000.0 );
    // resolution - retina display scaled x2
    int drawable_w,drawable_h;
    SDL_GL_GetDrawableSize(context->window, &drawable_w, &drawable_h);
    glUniform2f(shader->resolution_location, drawable_w, drawable_h );
    // optional attributes
    glUniform4fv(shader->optional_attributes_location, 1, layer->optional_shader_attributes );

    // Textures
    for(int i=0;i<8;i++) {
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
    GLfloat transforms[len][16];
    GLfloat mod_color[len][4];
    for(int i=0;i<len;i++){

      BiNode* node = context->rendering_nodes_queue[i];

      // texture_uv
      texture_z[i] = -1; // no-texture
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
      }

      //
      bi_mat4_copy(transforms[i], node->draw);

      // color
      mod_color[i][0] = node->color[0] / 255.0;
      mod_color[i][1] = node->color[1] / 255.0;
      mod_color[i][2] = node->color[2] / 255.0;
      mod_color[i][3] = node->color[3] / 255.0;
    }

    //
    // update vbo
    // orphaning: https://www.khronos.org/opengl/wiki/Buffer_Object_Streaming#Buffer_re-specification
    //
    // texture_z
    glBindBuffer(GL_ARRAY_BUFFER, shader->texture_z_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 1 * len, NULL, GL_DYNAMIC_DRAW); // XXX: float. not int.
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
    // color
    glBindBuffer(GL_ARRAY_BUFFER, shader->mod_color_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 4 * len, NULL, GL_DYNAMIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(GLfloat) * 4 * len, mod_color);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // set camera, projection, blend function
    set_projection(shader, context->w, context->h, layer->projection_centering);
    set_camera(shader, layer->camera_x, layer->camera_y);

    // blend function
    glBlendFuncSeparate(layer->blend_src,layer->blend_dst,layer->blend_alpha_src,layer->blend_alpha_dst);

    //
    // Draw instances
    //
    glBindVertexArray(shader->vao);
      glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, len);
    glBindVertexArray(0);
}

static void render_framebuffer(BiContext* context, GLuint texture, GLfloat attributes[4], BiShader* shader)
{
  BiTexture t;
  t.texture_id = texture;
  t.w = context->w;
  t.h = context->h;
  t._texture_unit = 0;

  BiTextureMapping m;
  bi_texture_mapping_init(&m);
  m.flip_vertical = true;
  m.texture = &t;
  bi_texture_mapping_set_bound(&m,0,0,m.texture->w,m.texture->h);

  BiNode n;
  bi_node_init(&n);
  bi_node_set_size(&n,t.w,t.h);
  n.texture_mapping = &m;
  bi_set_color(n.color,0xff,0xff,0xff,0xff);

  BiLayer l;
  bi_layer_init(&l);
  l.root = &n;
  l.textures[0] = &t;

  if(attributes){
    memcpy(l.optional_shader_attributes, attributes, sizeof(GLfloat)*4 );
  }
  if(shader) {
    l.shader = shader;
  }

  render_layer(context,&l);
}

static void render_layer_group(BiContext* context, BiLayerGroup *layer_group, GLuint parent_framebuffer)
{
  GLuint framebuffer = layer_group->framebuffer;

  // select framebuffer
  glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
  glClearColor(0,0,0,0);
  glClear(GL_COLOR_BUFFER_BIT);

  // render
  for( int i=0; i<layer_group->layers.size; i++ ) {
    if( ((BiLayerHeader*)layer_group->layers.objects[i])->type == BI_LAYER_TYPE_LAYER_GROUP ) {
      render_layer_group( context, layer_group->layers.objects[i], framebuffer );
    } else {
      render_layer( context, layer_group->layers.objects[i] );
    }
  }

  // chain post processing
  if( layer_group->post_processes.size > 0 ) {
    GLuint src_texture = layer_group->texture;
    for( int i=0; i<layer_group->post_processes.size; i++) {
      BiPostProcess* pp = layer_group->post_processes.objects[i];
      // target new framebuffer
      glBindFramebuffer(GL_FRAMEBUFFER, pp->framebuffer);
      glClearColor(0,0,0,0);
      glClear(GL_COLOR_BUFFER_BIT);
      // render
      render_framebuffer(context,src_texture,pp->optional_shader_attributes,pp->shader);
      src_texture = pp->texture;
    }
  }

  // reset
  glBindFramebuffer(GL_FRAMEBUFFER, parent_framebuffer);

  // finalize
  if( layer_group->post_processes.size > 0 ) {
    BiPostProcess *pp = layer_group->post_processes.objects[layer_group->post_processes.size-1];
    render_framebuffer(context,pp->texture,pp->optional_shader_attributes,pp->shader);
  }else{
    render_framebuffer(context,layer_group->texture,NULL,NULL);
  }
}

void bi_render(BiContext* context)
{
  // clear
  glClearColor( context->color[0]/255.f, context->color[1]/255.f, context->color[2]/255.f, context->color[3]/255.f );
  glClear(GL_COLOR_BUFFER_BIT);

  // reset stats
  context->profile.matrix_updated = 0;
  context->profile.rendering_nodes_queue_size = 0;

  // rendering
  render_layer_group(context,&context->layers,0);

  //
  SDL_GL_SwapWindow(context->window);
}
