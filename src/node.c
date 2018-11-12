#include <bi/node.h>
#include <bi/matrix.h>

#include <stdlib.h>
#include <string.h>

void bi_node_init(BiNode* n)
{
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
  n->color[0] = 0;
  n->color[1] = 0;
  n->color[2] = 0;
  n->color[3] = 0;

  bi_mat4_identity(n->transform);
  bi_mat4_identity(n->draw);

  n->matrix_cached = false;

  n->texture = NULL;

  n->children = NULL;
  n->children_size = 0;
  n->parent = NULL;

  // timers
  n->timers = NULL;
  n->timers_size = 0;

  // callbacks
  n->_on_update = NULL;
  n->_on_click = NULL;
  n->_on_move_cursor = NULL;
  n->_on_move_finger = NULL;
  n->_on_touch = NULL;
  n->_on_keyinput = NULL;
  n->_on_textinput = NULL;

  n->userdata = NULL;
}

//
// scene graph
//

void bi_add_node(BiNode* node,BiNode* child)
{
  node->children_size += 1;
  BiNode* *tmp = realloc( node->children, sizeof(BiNode*) * node->children_size );
  node->children = tmp;
  node->children[node->children_size-1] = child;
  child->parent = node;
  child->matrix_cached = false;
}

BiNode* bi_remove_node_index(BiNode* node,int index)
{
  if(index<0) return NULL;
  if(node->children_size==0) return NULL;

  BiNode* tmp = node->children[index];

  for(int i=index;i<node->children_size-1;i++){
    node->children[i] = node->children[i+1];
  }

  node->children_size -= 1;
  node->children = realloc( node->children, sizeof(BiNode*) * node->children_size );

  return tmp;
}

BiNode* bi_remove_node(BiNode* node,BiNode* child)
{
  int index = -1;
  for(int i=0;i<node->children_size;i++){
    if(node->children[i] == child){
      index = i;
      break;
    }
  }
  if(index<0){
    return NULL;
  }
  return bi_remove_node_index(node,index);
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

// Timers

void bi_node_add_timer(BiNode* node, BiTimer* timer)
{
    // TODO: duplicate check
    node->timers_size += 1;
    node->timers = realloc(node->timers, sizeof(BiTimer*)*node->timers_size);
    node->timers[node->timers_size-1] = timer;
}

BiTimer* bi_node_remove_timer_index(BiNode* node,int index)
{
  if(index<0) return NULL;
  if(node->timers_size==0) return NULL;

  BiTimer* tmp = node->timers[index];

  for(int i=index;i<node->timers_size-1;i++){
    node->timers[i] = node->timers[i+1];
  }

  node->timers_size -= 1;
  node->timers = realloc( node->timers, sizeof(BiTimer*) * node->timers_size );

  return tmp;
}

BiTimer* bi_node_remove_timer(BiNode* node, BiTimer* timer)
{
  int index = -1;
  for(int i=0;i<node->timers_size;i++){
    if(node->timers[i] == timer){
      index = i;
      break;
    }
  }
  if(index<0){
    return NULL;
  }
  return bi_node_remove_timer_index(node,index);
}


//
// callbacks
//

void bi_set_on_update(BiNode* node, on_update_callback callback, void *context)
{
  node->_on_update = callback;
  node->_on_update_context = context;
}

void bi_set_on_click(BiNode* node, on_click_callback callback, void *context)
{
  node->_on_click = callback;
  node->_on_click_context = context;
}

void bi_set_on_move_cursor(BiNode* node, on_move_cursor_callback callback, void *context)
{
  node->_on_move_cursor = callback;
  node->_on_move_cursor_context = context;
}

void bi_set_on_move_finger(BiNode* node, on_move_finger_callback callback, void *context)
{
  node->_on_move_finger = callback;
  node->_on_move_finger_context = context;
}

void bi_set_on_touch(BiNode* node, on_touch_callback callback, void *context)
{
  node->_on_touch = callback;
  node->_on_touch_context = context;
}

void bi_set_on_keyinput(BiNode* node, on_keyinput_callback callback, void *context)
{
  node->_on_keyinput = callback;
  node->_on_keyinput_context = context;
}

void bi_set_on_textinput(BiNode* node, on_textinput_callback callback, void *context)
{
  node->_on_textinput = callback;
  node->_on_textinput_context = context;
}

bool bi_node_has_callback(BiNode* n)
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
