#ifndef __BI_CORE_NODE_H__
#define __BI_CORE_NODE_H__

#include <stdbool.h>
#include <bi/bi_gl.h>
#include <bi/texture.h>
#include <bi/util.h>
#include <bi/timer.h>
#include <bi/array.h>

typedef struct _BiContext BiContext;
typedef struct _BiNode BiNode;

typedef void (*on_update_callback)(BiContext*, BiNode*, double); // delta_time
typedef bool (*on_click_callback)(BiContext*, BiNode*, int, int, int, bool); // x,y,button,pressed
typedef bool (*on_move_cursor_callback)(BiContext*, BiNode*, int, int); // x, y
typedef bool (*on_move_finger_callback)(BiContext*, BiNode*, float, float, int64_t); // x, y, fingerID
typedef bool (*on_touch_callback)(BiContext*, BiNode*, float, float, int64_t, bool); // x, y, fingerID, pressed
typedef bool (*on_keyinput_callback)(BiContext*, BiNode*, uint16_t, uint32_t, uint16_t, bool); // scancode, keycode, mod, pressed
typedef bool (*on_textinput_callback)(BiContext*, BiNode*, char*); // null-terminated UTF-8 text

struct _BiNode {
  int x;
  int y;
  int z;
  int w;
  int h;
  float angle;
  float scale_x;
  float scale_y;
  float anchor_x;
  float anchor_y;
  bool matrix_include_anchor_translate;
  bool visible;
  bool _final_visibility;

  uint8_t color[4];
  BiTextureMapping *texture_mapping;

  // matrix
  GLfloat transform[16];
  GLfloat draw[16];
  bool matrix_cached;

  BiNode *parent;
  Array children;
  bool children_order_cached;
  int _index;

  on_update_callback _on_update;
  on_click_callback _on_click;
  on_move_cursor_callback _on_move_cursor;
  on_move_finger_callback _on_move_finger;
  on_touch_callback _on_touch;
  on_keyinput_callback _on_keyinput;
  on_textinput_callback _on_textinput;

  BiTimerManager timers;
  double time_scale;

  void* userdata;
};


extern BiNode* bi_node_init(BiNode* n);

// scene graph
static inline BiNode* bi_node_child_at(BiNode* node,int index){ return (BiNode*)node->children.objects[index]; }
extern void bi_node_add_node(BiNode* node,BiNode* child);
extern BiNode* bi_node_remove_at(BiNode* node,int index);
extern BiNode* bi_node_remove_node(BiNode* node,BiNode* child);
extern void bi_node_sort(BiNode* node);

// geometry
extern void bi_node_set_position(BiNode* n, int x, int y);
static inline void bi_node_set_x(BiNode* n, int x) { bi_node_set_position(n,x,n->y); }
static inline void bi_node_set_y(BiNode* n, int y) { bi_node_set_position(n,n->x,y); }
extern void bi_node_set_z(BiNode* n, int z);
extern void bi_node_set_size(BiNode* n, int w, int h);
static inline void bi_node_set_w(BiNode* n, int w) { bi_node_set_size(n,w,n->h); }
static inline void bi_node_set_h(BiNode* n, int h) { bi_node_set_size(n,n->w,h); }
extern void bi_node_set_scale(BiNode* n, float x, float y);
static inline void bi_node_set_scale_x(BiNode* n, float x) { bi_node_set_scale(n,x,n->scale_y); }
static inline void bi_node_set_scale_y(BiNode* n, float y) { bi_node_set_scale(n,n->scale_x,y); }
static inline void bi_node_set_anchor(BiNode* n, float x, float y) { n->anchor_x=x; n->anchor_y=y; }
extern void bi_node_set_angle(BiNode* n, float angle);
static inline void bi_node_set_degree(BiNode* n, float degree) { bi_node_set_angle(n,degree*(M_PI/180.f)); }
static inline double bi_node_get_degree(BiNode* n) { return n->angle*(180.f/M_PI); }
extern void bi_node_set_matrix_include_anchor_translate(BiNode* n, bool matrix_include_anchor_translate);
extern void bi_node_transform_local(BiNode* node, int x, int y, int *lx, int*ly);
extern bool bi_node_inside(BiNode* node, int x, int y);

// event handler
static inline void bi_node_set_on_update(BiNode* node, on_update_callback callback){node->_on_update = callback;}
static inline void bi_node_set_on_click(BiNode* node, on_click_callback callback){node->_on_click = callback;}
static inline void bi_node_set_on_move_cursor(BiNode* node, on_move_cursor_callback callback){node->_on_move_cursor = callback;}
static inline void bi_node_set_on_move_finger(BiNode* node, on_move_finger_callback callback){node->_on_move_finger = callback;}
static inline void bi_node_set_on_touch(BiNode* node, on_touch_callback callback){node->_on_touch = callback;}
static inline void bi_node_set_on_keyinput(BiNode* node, on_keyinput_callback callback){node->_on_keyinput = callback;}
static inline void bi_node_set_on_textinput(BiNode* node, on_textinput_callback callback){node->_on_textinput = callback;}

#endif
