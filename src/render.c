#include <bi/render.h>
#include <bi/node.h>
#include <bi/context.h>
#include <bi/matrix.h>
#include <bi/layer.h>

#include <GL/glew.h>
#include <math.h>

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

static void draw(BiContext* context, BiNode* n, bool visible)
{
    // add callback
    if( n->_on_update != NULL || bi_node_has_callback(n) || n->timers_size > 0 ) {
      context->callback_planned_nodes[context->callback_planned_nodes_size] = n;
      context->callback_planned_nodes_size++;
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
    for(int i=0;i<n->children_size;i++){
      if( n->children[i]->matrix_cached == true && matrix_update_require ) {
          n->children[i]->matrix_cached = false;
      }
      draw(context, n->children[i], visible && n->visible ? true : false);
    }
}

void bi_render_layer(BiContext* context,BiLayer* layer)
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
    BiShader* shader = &context->shader;

    //
    // copy to buffer
    //
    GLuint texture_units[8] = { 0 }; // texture unit to texture id
    GLfloat uv[4*len]; // [ left, top, right, bottom ]
    GLfloat texture_z[len];
    GLfloat transforms[len][16];
    GLfloat mod_color[len][4];
    for(int i=0;i<len;i++){

      BiNode* node = context->rendering_nodes_queue[i];

      // texture_uv
      texture_z[i] = -1; // no-texture
      if(node->texture != NULL) {
        // Left-Top is 0-0, Right-Bottom is 1-1.
        BiTexture *t = node->texture;
        uv[i*4+0] = t->boundary[0]; // Left
        uv[i*4+1] = t->boundary[1]; // Top
        uv[i*4+2] = t->boundary[2]; // Right
        uv[i*4+3] = t->boundary[3]; // Bottom
        if( 0 <= t->texture_image->texture_unit && t->texture_image->texture_unit < 8 ) {
          texture_z[i] = t->texture_image->texture_unit;
          texture_units[t->texture_image->texture_unit] = t->texture_image->texture_id;
        }
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

    //
    // texture
    //

    for(int i=0;i<8;i++) {
      //
      glUniform1i( shader->texture_locations[i], i );
      // texture bind
      glActiveTexture(GL_TEXTURE0+i);
      glBindTexture(GL_TEXTURE_2D, texture_units[i]);
    }

    // set camera, projection, blend function
    bi_set_projection(&context->shader, context->w, context->h, layer->projection_centering);
    bi_set_camera(&context->shader, layer->camera_x, layer->camera_y);

    // blend function
    glBlendFunc(layer->blend_src,layer->blend_dst);

    //
    // Draw instances
    //
    glBindVertexArray(shader->vao);
      // XXX: not glDrawArraysInstancedANGLE in emscripten
      glDrawArraysInstancedARB(GL_TRIANGLE_STRIP, 0, 4, len);
    glBindVertexArray(0);
}
