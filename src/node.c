#include <bi/node.h>
#include <bi/matrix.h>
#include <stdlib.h>
#include <string.h>

BiNode* bi_node_init(BiNode* n)
{
  bi_raw_node_init((BiRawNode*)n,BI_NODE_TYPE_NODE);
  n->x = 0;
  n->y = 0;
  n->w = 0;
  n->h = 0;
  n->angle = 0;
  n->scale_x = 1;
  n->scale_y = 1;
  n->anchor_x = 0;
  n->anchor_y = 0;
  n->matrix_include_anchor_translate = false;
  n->visible = true;
  n->_final_visibility = true;

  // color
  bi_set_color(n->color,0,0,0,0);
  n->opacity = 1.0;

  bi_mat4_identity(n->transform);
  bi_mat4_identity(n->draw);

  n->matrix_cached = false;

  n->texture_mapping = NULL;

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
  child->matrix_cached = false;
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
  if(n->x != x || n->y != y) {
    n->matrix_cached = false;
  }
  n->x = x;
  n->y = y;
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
  if(n->w != w || n->h != h) {
    n->matrix_cached = false;
  }
  n->w = w;
  n->h = h;
}

void bi_node_set_scale(BiNode* n, float x, float y)
{
  if(n->scale_x != x || n->scale_y != y) {
    n->matrix_cached = false;
  }
  n->scale_x = x;
  n->scale_y = y;
}

void bi_node_set_angle(BiNode* n, float angle)
{
  if(n->angle != angle) {
    n->matrix_cached = false;
  }
  n->angle = angle;
}

void bi_node_set_matrix_include_anchor_translate(BiNode* n, bool matrix_include_anchor_translate)
{
    if(n->matrix_include_anchor_translate != matrix_include_anchor_translate) {
      n->matrix_cached = false;
    }
    n->matrix_include_anchor_translate = matrix_include_anchor_translate;
}

void bi_node_transform_local(BiNode* node, int x, int y, int *lx, int*ly)
{
  GLfloat m[16];
  bi_mat4_invert(m, node->transform);

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

    if(node->matrix_include_anchor_translate){
      l = 0;
      r = node->w;
      b = 0;
      t = node->h;
    }

    if( l <= lx && lx <= r && b <= ly && ly <= t ) {
      return true;
    }
    return false;
}

//

bool bi_node_update_matrix(BiNode* n)
{
  if( n->matrix_cached ) {
    return false;
  }
  n->matrix_cached = true;

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

  return true;
}
