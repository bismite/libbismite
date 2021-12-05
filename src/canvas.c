#include <bi/canvas.h>
#include <bi/matrix.h>
#include <bi/bi_gl.h>
#include <bi/render.h>
#include <bi/node.h>
#include <bi/shader.h>
#include <bi/context.h>

BiCanvas* bi_canvas_init(BiCanvas* canvas,int w,int h)
{
  canvas->w = w;
  canvas->h = h;
  bi_framebuffer_init(&canvas->framebuffer,w,h);
  bi_set_blend_factor(&canvas->blend_factor,GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA,GL_ONE,GL_ONE_MINUS_SRC_ALPHA);
  canvas->shader = NULL;
  for(int i=0;i<4;i++) {
    canvas->shader_attributes[i] = 0;
  }
  for(int i=0;i<BI_LAYER_MAX_TEXTURES;i++) {
    canvas->textures[i] = NULL;
  }
  // default texture
  uint8_t pixels[4] = {0,0,0,0};
  glGenTextures(1, &canvas->default_texture);
  glBindTexture(GL_TEXTURE_2D, canvas->default_texture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glBindTexture(GL_TEXTURE_2D, 0);
  return canvas;
}

static void queuing(BiNode* n,Array* queue)
{
  // skip: invisible, zero-size node, transparent node
  if( n->visible==true && n->w!=0 && n->h!=0 && n->opacity>0 ) {
    array_add_object(queue,n);
  }else{
    return;
  }

  //
  bool matrix_update_require = false;
  if( n->matrix_cached == false ) {
      bi_render_update_matrix(n);
      n->matrix_cached = true;
      matrix_update_require = true;
  }

  //
  bi_render_node_sort(n);
  for( int i=0; i<n->children.size; i++ ){
    if( bi_node_child_at(n,i)->matrix_cached == true && matrix_update_require ) {
        bi_node_child_at(n,i)->matrix_cached = false;
    }
    queuing(bi_node_child_at(n,i), queue);
  }
}

void bi_canvas_draw(BiCanvas* canvas,BiNode* node)
{
  Array queue;
  array_init(&queue);
  queuing(node,&queue);

  const size_t len = queue.size;
  if(len==0) return;

  // viewport setting
  GLint original_viewport[4];
  glGetIntegerv(GL_VIEWPORT,original_viewport);
  glViewport(0,0,canvas->w,canvas->h);

  // bind
  glBindFramebuffer(GL_FRAMEBUFFER, canvas->framebuffer.framebuffer_id);

  // set shader
  BiShader* shader = canvas->shader;
  glUseProgram(shader->program_id);

  // // time
  // glUniform1f(shader->time_location, (context->program_start_at - context->_frame_start_at)/1000.0 );
  // resolution
  glUniform2f(shader->resolution_location, canvas->w, canvas->h );
  // // scale
  // int drawable_w,drawable_h;
  // SDL_GL_GetDrawableSize(context->window, &drawable_w, &drawable_h);
  // glUniform1f(shader->scale_location, (float)drawable_h / context->h );
  // optional attributes
  glUniform4fv(shader->optional_attributes_location, 1, canvas->shader_attributes );

  // Textures
  for(int i=0;i<BI_LAYER_MAX_TEXTURES;i++) {
    // texture bind
    glActiveTexture(GL_TEXTURE0+i);
    if( canvas->textures[i] == NULL ) {
      glBindTexture(GL_TEXTURE_2D, canvas->default_texture);
    }else{
      canvas->textures[i]->_texture_unit = i;
      glBindTexture(GL_TEXTURE_2D, canvas->textures[i]->texture_id);
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
    BiNode* node = queue.objects[i];
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
  bi_shader_set_camera(shader, canvas->w, canvas->h, 0,0, true);

  // blend function
  glBlendFuncSeparate(
    canvas->blend_factor.src,
    canvas->blend_factor.dst,
    canvas->blend_factor.alpha_src,
    canvas->blend_factor.alpha_dst
  );

  //
  // Draw instances
  //
  glBindVertexArray(shader->vao);
    glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, len);
  glBindVertexArray(0);

  // unbind
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  // restore viewport
  glViewport(0,0,original_viewport[2],original_viewport[3]);
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
  texture->_texture_unit = 0;
  return texture;
}

void bi_canvas_save_png_image(BiCanvas* canvas,const char *filename)
{
  uint8_t* pixels = malloc(8*canvas->w*canvas->h);
  glBindTexture(GL_TEXTURE_2D,canvas->framebuffer.texture_id);
#ifdef __EMSCRIPTEN__
  glReadPixels(0,0,canvas->w,canvas->h,GL_RGBA,GL_UNSIGNED_BYTE,pixels);
#else
  glGetTexImage(GL_TEXTURE_2D,0,GL_RGBA,GL_UNSIGNED_BYTE,pixels);
#endif
  glBindTexture(GL_TEXTURE_2D,0);
  int pitch = 4*canvas->w;
  SDL_Surface* s = SDL_CreateRGBSurfaceWithFormatFrom(pixels,canvas->w,canvas->h,32,pitch,SDL_PIXELFORMAT_RGBA32);
  IMG_SavePNG(s,filename);
  SDL_FreeSurface(s);
  free(pixels);
}
