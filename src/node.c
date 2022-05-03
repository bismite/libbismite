#include <bi/node.h>
#include <bi/matrix.h>
#include <stdlib.h>
#include <string.h>

BiNode* bi_node_init(BiNode* n)
{
  bi_raw_node_init((BiRawNode*)n,BI_NODE_TYPE_NODE);
  n->_x = n->_y = n->_w = n->_h = 0;
  n->_angle = 0;
  n->_scale_x = 1;
  n->_scale_y = 1;
  n->_anchor_x = 0;
  n->_anchor_y = 0;
  n->visible = true;
  n->_final_visibility = true;

  // color
  bi_set_color(n->color,0,0,0,0);
  n->opacity = 1.0;

  bi_mat4_identity(n->_transform_matrix);
  bi_mat4_identity(n->_draw_matrix);

  n->_transform_matrix_cached = false;

  // Texture
  n->_texture = NULL;
  n->_tx = n->_ty = n->_tw = n->_th = 0;
  n->_texture_uv_left = 0;
  n->_texture_uv_top = 0;
  n->_texture_uv_right = 0;
  n->_texture_uv_bottom = 0;
  n->_texture_cropped = false;
  n->_cx = n->_cy = n->_ow = n->_oh = 0;
  n->_texture_flip_horizontal = false;
  n->_texture_flip_vertical = false;

  //
  array_init(&n->children);
  n->parent = NULL;
  n->children_order_cached = true;

  // callbacks
  n->_on_click = NULL;
  n->_on_move_cursor = NULL;
  n->_on_move_finger = NULL;
  n->_on_touch = NULL;
  n->_on_keyinput = NULL;
  n->_on_textinput = NULL;

  n->userdata = NULL;

  return n;
}

//
// scene graph
//

void bi_node_add_node(BiNode* node,BiNode* child)
{
  array_add_object(&node->children,child);
  node->children_order_cached = false;
  child->parent = node;
  child->_transform_matrix_cached = false;
}

BiNode* bi_node_remove_at(BiNode* node,int index)
{
  return array_remove_object_at(&node->children,index);
}

BiNode* bi_node_remove_node(BiNode* node,BiNode* child)
{
  return array_remove_object(&node->children,child);
}

BiNode* bi_node_remove_from_parent(BiNode* node)
{
  if(node && node->parent) return bi_node_remove_node(node->parent,node);
  return NULL;
}

//
// geometry
//

void bi_node_set_position(BiNode* n, int x, int y)
{
  if(n->_x != x || n->_y != y) {
    n->_transform_matrix_cached = false;
  }
  n->_x = x;
  n->_y = y;
}

void bi_node_set_z(BiNode* n, int z)
{
  n->z = z;
  if( n->parent ) {
    n->parent->children_order_cached = false;
  }
}

void bi_node_set_size(BiNode* n, int w, int h)
{
  if(n->_w != w || n->_h != h) {
    n->_transform_matrix_cached = false;
  }
  n->_w = w;
  n->_h = h;
}

void bi_node_set_scale(BiNode* n, float x, float y)
{
  if(n->_scale_x != x || n->_scale_y != y) {
    n->_transform_matrix_cached = false;
  }
  n->_scale_x = x;
  n->_scale_y = y;
}

void bi_node_set_anchor(BiNode* n, float x, float y)
{
  n->_anchor_x=x;
  n->_anchor_y=y;
  n->_draw_matrix_cached = false;
}

void bi_node_set_angle(BiNode* n, float angle)
{
  if(n->_angle != angle) {
    n->_transform_matrix_cached = false;
  }
  n->_angle = angle;
}

void bi_node_transform_local(BiNode* node, int x, int y, int *lx, int*ly)
{
  GLfloat m[16];
  bi_mat4_invert(m, node->_transform_matrix);

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
    GLfloat l = - node->_anchor_x * node->_w;
    GLfloat r = l + node->_w;
    GLfloat b = - node->_anchor_y * node->_h;
    GLfloat t = b + node->_h;

    if( l <= lx && lx <= r && b <= ly && ly <= t ) {
      return true;
    }
    return false;
}

// Matrix

static inline void update_draw_matrix(BiNode* n)
{
  if(n->_texture_cropped) {
    // offset
    GLfloat x = n->_texture_flip_horizontal ? n->_ow-n->_cx-n->_tw : n->_cx;
    GLfloat y = n->_texture_flip_vertical   ? n->_cy : n->_oh-n->_cy-n->_th;
    // normalize
    x = x * n->_w / n->_ow;
    y = y * n->_h / n->_oh;
    // add anchor
    x += - n->_anchor_x * n->_w;
    y += - n->_anchor_y * n->_h;
    // scale with normalize
    GLfloat w = (GLfloat)n->_tw * n->_w / n->_ow;
    GLfloat h = (GLfloat)n->_th * n->_h / n->_oh;
    // trans & scale
    GLfloat tmp[16] = {
      w,  0,  0,  0,
      0,  h,  0,  0,
      0,  0,  1,  0,
      x,  y,  0,  1
    };
    bi_mat4_multiply(tmp,n->_transform_matrix,n->_draw_matrix);
  }else{
    GLfloat lx = - n->_anchor_x * n->_w;
    GLfloat ly = - n->_anchor_y * n->_h;
    GLfloat lw = n->_w;
    GLfloat lh = n->_h;
    GLfloat tmp[16] = {
      lw,   0,  0,  0,
       0,  lh,  0,  0,
       0,   0,  1,  0,
      lx,  ly,  0,  1
    };
    bi_mat4_multiply(tmp, n->_transform_matrix, n->_draw_matrix);
  }
}

static inline void update_transform_matrix(BiNode* n)
{
  GLfloat tx = n->_x;
  GLfloat ty = n->_y;
  GLfloat sx = n->_scale_x ;
  GLfloat sy = n->_scale_y ;
  // XXX: non-zero angle calculation is super slow!
  GLfloat cos_v = cos(n->_angle);
  GLfloat sin_v = sin(n->_angle);

  // matrix chain
  if(n->parent == NULL){
    bi_mat4_identity( n->_transform_matrix );
  }else{
    bi_mat4_copy( n->_transform_matrix, n->parent->_transform_matrix );
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

  bi_mat4_multiply(trans,n->_transform_matrix,n->_transform_matrix);
  bi_mat4_multiply(rotation,n->_transform_matrix,n->_transform_matrix);
  bi_mat4_multiply(scale,n->_transform_matrix,n->_transform_matrix);
}

bool bi_node_update_matrix(BiNode* n)
{
  if( n->_transform_matrix_cached == false ) {
    update_transform_matrix(n);
    update_draw_matrix(n);
    n->_transform_matrix_cached = true;
    n->_draw_matrix_cached = true;
    return true;
  }
  if( n->_draw_matrix_cached == false ) {
    update_draw_matrix(n);
    n->_draw_matrix_cached = true;
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
  n->_texture = t;
  n->_tx = tx;
  n->_ty = ty;
  n->_tw = tw;
  n->_th = th;
  // left, top, right, bottom
  n->_texture_uv_left  = (GLfloat)tx      / t->w;
  n->_texture_uv_top   = (GLfloat)(ty+th) / t->h;
  n->_texture_uv_right = (GLfloat)(tx+tw) / t->w;
  n->_texture_uv_bottom= (GLfloat)ty      / t->h;
  // crop position and original image size
  n->_cx = cx;
  n->_cy = cy;
  n->_ow = ow;
  n->_oh = oh;
  if(ow>0 && oh>0 && (tw!=ow || th!=oh)) {
    n->_texture_cropped = true;
    n->_draw_matrix_cached = false;
  } else {
    if(n->_texture_cropped) n->_draw_matrix_cached = false;
    n->_texture_cropped = false;
  }
}

void bi_node_unset_texture(BiNode* n)
{
  n->_draw_matrix_cached = false;
  n->_texture = NULL;
  n->_texture_cropped = false;
}
