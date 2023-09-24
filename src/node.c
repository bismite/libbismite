#include <bi/node.h>
#include "matrix/matrix.h"
#include <stdlib.h>
#include <string.h>

BiNode* bi_node_init(BiNode* n)
{
  bi_node_base_init((BiNodeBase*)n,BI_NODE);
  n->x = n->y = n->w = n->h = 0;
  n->angle = 0;
  n->scale_x = 1;
  n->scale_y = 1;
  n->anchor_x = 0;
  n->anchor_y = 0;
  n->visible = true;
  n->final_visibility = true;

  // color
  n->color_tint = RGBA32(0);
  n->color_modulate = RGBA32(0xffffffff);

  //
  for(int i=0;i<16;i++) {
    n->shader_extra_data[i] = 0;
  }

  bi_mat4_identity(n->transform_matrix);
  bi_mat4_identity(n->draw_matrix);

  n->transform_matrix_cached = false;

  // Texture
  n->texture = NULL;
  n->tx = n->ty = n->tw = n->th = 0;
  n->texture_uv_left = 0;
  n->texture_uv_top = 0;
  n->texture_uv_right = 0;
  n->texture_uv_bottom = 0;
  n->texture_cropped = false;
  n->cx = n->cy = n->ow = n->oh = 0;
  n->texture_flip_horizontal = false;
  n->texture_flip_vertical = false;

  // callbacks
  n->on_click = NULL;
  n->on_move_cursor = NULL;
  n->on_move_finger = NULL;
  n->on_touch = NULL;
  n->on_keyinput = NULL;
  n->on_textinput = NULL;

  n->userdata = NULL;

  return n;
}

//
// scene graph
//

void bi_node_add_node(BiNode* node,BiNode* child)
{
  array_add_object(&node->children,child);
  child->parent = (BiNodeBase*)node;
  child->transform_matrix_cached = false;
}

BiNode* bi_node_remove_at(BiNode* node,int index)
{
  BiNode *tmp = array_remove_object_at(&node->children,index);
  if(tmp){
    tmp->parent = NULL;
    return tmp;
  }
  return NULL;
}

BiNode* bi_node_remove_node(BiNode* node,BiNode* child)
{
  if( child == array_remove_object(&node->children,child) ){
    child->parent = NULL;
  }
  return child;
}

BiNode* bi_node_remove_from_parent(BiNode* node)
{
  if(node && node->parent) {
    return bi_node_remove_node((BiNode*)node->parent,node);
  }
  return NULL;
}

//
// geometry
//

void bi_node_set_position(BiNode* n, int x, int y)
{
  if(n->x != x || n->y != y) {
    n->transform_matrix_cached = false;
  }
  n->x = x;
  n->y = y;
}

void bi_node_set_z(BiNode* n, int z)
{
  n->z = z;
  if( n->parent ) {
    ((BiNode*)n->parent)->children.order_cached = false;
  }
}

void bi_node_set_size(BiNode* n, int w, int h)
{
  if(n->w != w || n->h != h) {
    n->transform_matrix_cached = false;
  }
  n->w = w;
  n->h = h;
}

void bi_node_set_scale(BiNode* n, float x, float y)
{
  if(n->scale_x != x || n->scale_y != y) {
    n->transform_matrix_cached = false;
  }
  n->scale_x = x;
  n->scale_y = y;
}

void bi_node_set_anchor(BiNode* n, float x, float y)
{
  n->anchor_x=x;
  n->anchor_y=y;
  n->draw_matrix_cached = false;
}

void bi_node_set_angle(BiNode* n, float angle)
{
  if(n->angle != angle) {
    n->transform_matrix_cached = false;
  }
  n->angle = angle;
}

void bi_node_transform_local(BiNode* node, int x, int y, int *lx, int*ly)
{
  GLfloat m[16];
  bi_mat4_invert(m, node->transform_matrix);

  float v[4] = {0};
  v[0] = x;
  v[1] = y;
  v[2] = 0;
  v[3] = 1;

  GLfloat lxf = v[0] * m[0] + v[1] * m[4] + v[2] * m[8] + v[3] * m[12];
  GLfloat lyf = v[0] * m[1] + v[1] * m[5] + v[2] * m[9] + v[3] * m[13];

  *lx = lxf;
  *ly = lyf;
}

bool bi_node_inside(BiNode* node, int x, int y)
{
    int lx,ly;
    bi_node_transform_local(node,x,y,&lx,&ly);

    // left right top bottom
    GLfloat l = - node->anchor_x * node->w;
    GLfloat r = l + node->w;
    GLfloat b = - node->anchor_y * node->h;
    GLfloat t = b + node->h;

    if( l <= lx && lx <= r && b <= ly && ly <= t ) {
      return true;
    }
    return false;
}

// Matrix

static inline void update_draw_matrix(BiNode* n)
{
  if(n->texture_cropped) {
    // offset
    GLfloat x = n->texture_flip_horizontal ? n->ow-n->cx-n->tw : n->cx;
    GLfloat y = n->texture_flip_vertical   ? n->cy : n->oh-n->cy-n->th;
    // normalize
    x = x * n->w / n->ow;
    y = y * n->h / n->oh;
    // add anchor
    x += - n->anchor_x * n->w;
    y += - n->anchor_y * n->h;
    // scale with normalize
    GLfloat w = (GLfloat)n->tw * n->w / n->ow;
    GLfloat h = (GLfloat)n->th * n->h / n->oh;
    // trans & scale
    GLfloat tmp[16] = {
      w,  0,  0,  0,
      0,  h,  0,  0,
      0,  0,  1,  0,
      x,  y,  0,  1
    };
    bi_mat4_multiply(tmp,n->transform_matrix,n->draw_matrix);
  }else{
    GLfloat lx = - n->anchor_x * n->w;
    GLfloat ly = - n->anchor_y * n->h;
    GLfloat lw = n->w;
    GLfloat lh = n->h;
    GLfloat tmp[16] = {
      lw,   0,  0,  0,
       0,  lh,  0,  0,
       0,   0,  1,  0,
      lx,  ly,  0,  1
    };
    bi_mat4_multiply(tmp, n->transform_matrix, n->draw_matrix);
  }
}

static inline void update_transform_matrix(BiNode* n)
{
  GLfloat tx = n->x;
  GLfloat ty = n->y;
  GLfloat sx = n->scale_x ;
  GLfloat sy = n->scale_y ;
  // XXX: non-zero angle calculation is super slow!
  GLfloat cos_v = cos(n->angle);
  GLfloat sin_v = sin(n->angle);

  // matrix chain
  if( n->parent && n->parent->class == BI_NODE ){
    bi_mat4_copy( n->transform_matrix, ((BiNode*)n->parent)->transform_matrix );
  }else{
    bi_mat4_identity( n->transform_matrix );
  }

  GLfloat trans[16] = {
    1.0, 0.0, 0.0, 0.0,
    0.0, 1.0, 0.0, 0.0,
    0.0, 0.0, 1.0, 0.0,
     tx,  ty, 0.0, 1.0
  };

  // convert to CCW
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

  bi_mat4_multiply(trans,n->transform_matrix,n->transform_matrix);
  bi_mat4_multiply(rotation,n->transform_matrix,n->transform_matrix);
  bi_mat4_multiply(scale,n->transform_matrix,n->transform_matrix);
}

bool bi_node_update_matrix(BiNode* n)
{
  if( n->transform_matrix_cached == false ) {
    update_transform_matrix(n);
    update_draw_matrix(n);
    n->transform_matrix_cached = true;
    n->draw_matrix_cached = true;
    return true;
  }
  if( n->draw_matrix_cached == false ) {
    update_draw_matrix(n);
    n->draw_matrix_cached = true;
    return false;
  }
  // nop
  return false;
}

//
// Texture
//
void bi_node_set_texture(BiNode* n, BiTexture* t, uint16_t tx, uint16_t ty, uint16_t tw,  uint16_t th)
{
  bi_node_set_cropped_texture(n,t,tx,ty,tw,th,0,0,tw,th);
}

void bi_node_set_cropped_texture(BiNode* n, BiTexture* t,
  uint16_t tx, uint16_t ty, uint16_t tw,  uint16_t th,
  uint16_t cx, uint16_t cy, uint16_t ow, uint16_t oh )
{
  n->texture = t;
  n->tx = tx;
  n->ty = ty;
  n->tw = tw;
  n->th = th;
  // left, top, right, bottom
  n->texture_uv_left  = (GLfloat)tx      / t->w;
  n->texture_uv_top   = (GLfloat)(ty+th) / t->h;
  n->texture_uv_right = (GLfloat)(tx+tw) / t->w;
  n->texture_uv_bottom= (GLfloat)ty      / t->h;
  // crop position and original image size
  n->cx = cx;
  n->cy = cy;
  n->ow = ow;
  n->oh = oh;
  if(ow>0 && oh>0 && (tw!=ow || th!=oh)) {
    n->texture_cropped = true;
    n->draw_matrix_cached = false;
  } else {
    if(n->texture_cropped) n->draw_matrix_cached = false;
    n->texture_cropped = false;
  }
}

void bi_node_unset_texture(BiNode* n)
{
  n->draw_matrix_cached = false;
  n->texture = NULL;
  n->texture_cropped = false;
}
