#ifndef __BINODE_H__
#define __BINODE_H__

#include <stdbool.h>
#include <bi/node_base.h>
#include <bi/timer.h>
#include <bi/bi_gl.h>
#include <bi/util.h>
#include <bi/array.h>
#include <bi/color.h>

typedef struct _BiContext BiContext;
typedef struct _BiNode BiNode;
typedef struct _BiTexture BiTexture;
typedef struct _BiFramebuffer BiFramebuffer;

typedef bool (*on_click_callback)(BiContext*, BiNode*, int, int, int, bool); // x,y,button,pressed
typedef bool (*on_move_cursor_callback)(BiContext*, BiNode*, int, int); // x, y
typedef bool (*on_move_finger_callback)(BiContext*, BiNode*, float, float, int64_t); // x, y, fingerID
typedef bool (*on_touch_callback)(BiContext*, BiNode*, float, float, int64_t, bool); // x, y, fingerID, pressed
typedef bool (*on_keyinput_callback)(BiContext*, BiNode*, uint16_t, uint32_t, uint16_t, bool); // scancode, keycode, mod, pressed
typedef bool (*on_textinput_callback)(BiContext*, BiNode*, char*); // null-terminated UTF-8 text

struct _BiNode {
  BI_NODE_HEADER;
  int x, y, w, h;
  float angle;
  float scale_x;
  float scale_y;
  float anchor_x;
  float anchor_y;
  bool visible;
  bool _final_visibility;

  // colors
  BiColor color;
  BiColor tint;

  //
  GLfloat shader_extra_data[16];

  // matrix
  GLfloat transform_matrix[16];
  GLfloat draw_matrix[16];
  bool transform_matrix_cached;
  bool draw_matrix_cached;

  // Texture
  BiTexture *_texture;
  GLfloat texture_uv[4]; // left,bottom,right,top
  GLfloat texture_crop_uv[4]; // left,bottom,right,top
  bool texture_flip_horizontal;
  bool texture_flip_vertical;

  // Framebuffer
  BiFramebuffer *framebuffer;

  // Event Handlers
  on_click_callback on_click;
  on_move_cursor_callback on_move_cursor;
  on_move_finger_callback on_move_finger;
  on_touch_callback on_touch;
  on_keyinput_callback on_keyinput;
  on_textinput_callback on_textinput;
};


extern BiNode* bi_node_init(BiNode* n);

static inline BiNode* bi_node_p(void* n){
  if(n==NULL) return NULL;
  if( ((BiNodeBase*)n)->class != BI_NODE ) return NULL;
  return n;
}

// color
static inline void bi_node_set_opacity(BiNode* node,uint8_t opacity) { node->color.a = opacity; }
static inline uint8_t bi_node_get_opacity(BiNode* node) { return node->color.a; }

// position
extern void bi_node_set_position(BiNode* n, int x, int y);
static inline void bi_node_set_x(BiNode* n, int x) { bi_node_set_position(n,x,n->y); }
static inline void bi_node_set_y(BiNode* n, int y) { bi_node_set_position(n,n->x,y); }
extern void bi_node_transform_local(BiNode* node, int x, int y, int *lx, int*ly);
extern bool bi_node_inside(BiNode* node, int x, int y);
// size
extern void bi_node_set_size(BiNode* n, int w, int h);
static inline void bi_node_set_w(BiNode* n, int w) { bi_node_set_size(n,w,n->h); }
static inline void bi_node_set_h(BiNode* n, int h) { bi_node_set_size(n,n->w,h); }
// scale
extern void bi_node_set_scale(BiNode* n, float x, float y);
static inline void bi_node_set_scale_x(BiNode* n, float x) { bi_node_set_scale(n,x,n->scale_y); }
static inline void bi_node_set_scale_y(BiNode* n, float y) { bi_node_set_scale(n,n->scale_x,y); }
// anchor
extern void bi_node_set_anchor(BiNode* n, float x, float y);
static inline void bi_node_set_anchor_x(BiNode* n, float x){ bi_node_set_anchor(n,x,n->anchor_y); }
static inline void bi_node_set_anchor_y(BiNode* n, float y){ bi_node_set_anchor(n,n->anchor_x,y); }
// angle
extern void bi_node_set_angle(BiNode* n, float angle);
static inline void bi_node_set_degree(BiNode* n, float degree) { bi_node_set_angle(n,degree*(M_PI/180.f)); }
static inline double bi_node_get_degree(BiNode* n) { return n->angle*(180.f/M_PI); }


// Matrix
extern bool bi_node_update_matrix(BiNode* n);

// Texture
static inline BiTexture* bi_node_get_texture(BiNode* n){ return n->_texture; }
extern void bi_node_set_texture(BiNode*, BiTexture*, uint16_t tx, uint16_t ty, uint16_t tw,  uint16_t th);
extern void bi_node_unset_texture(BiNode*);
//  Original Image and Crop   Cropped Texture in Sprite Sheet
//  0,0 -----------------    0,0 -----------------
//   |                  |     |                  |  ^
//   |     cx,cy---- ^  |     |     tx,ty---- ^  |  |
//   |     |Cropped| th |     |     |Cropped| th | tex.h
//   |     --------- v  |     |     --------- v  |  |
//   |     <-- tw-->    |     |     <-- tw-->    |  v
//   --------------- ow,oh    --------------------
//                                <- tex.w ->
extern void bi_node_set_cropped_texture(BiNode*, BiTexture*,
  uint16_t tx, uint16_t ty, uint16_t tw,  uint16_t th,
  uint16_t cx, uint16_t cy, uint16_t ow, uint16_t oh );
//  UV and Crop in Sprite Sheet
//  -----------------------------1/1   Sprite Node
//  |     --------------- r1,t1    |    ------------- r1,t1
//  |     |                  |     |    |###############|
//  |     |   ------- r2,t2  |     |    |###---------###|
//  |     |   | Cropped |    |     | -> |###|Cropped|###|
//  |     |  l2,b2 ------    |     |    |###---------###|
//  |     |                  |     |    |###############|
//  |     l1,b1 --------------     |   l1,b1 ------------
//  0,0 ----------------------------
//  Note: Images are loaded into memory ‘upside down’.
extern void bi_node_set_texture_mapping(BiNode*, BiTexture*,
  GLfloat l1, GLfloat b1, GLfloat r1, GLfloat t1,
  GLfloat l2, GLfloat b2, GLfloat r2, GLfloat t2 );

// event handler
static inline void bi_node_set_on_click(BiNode* node, on_click_callback callback){node->on_click=callback;}
static inline void bi_node_set_on_move_cursor(BiNode* node, on_move_cursor_callback callback){node->on_move_cursor=callback;}
static inline void bi_node_set_on_move_finger(BiNode* node, on_move_finger_callback callback){node->on_move_finger=callback;}
static inline void bi_node_set_on_touch(BiNode* node, on_touch_callback callback){node->on_touch=callback;}
static inline void bi_node_set_on_keyinput(BiNode* node, on_keyinput_callback callback){node->on_keyinput=callback;}
static inline void bi_node_set_on_textinput(BiNode* node, on_textinput_callback callback){node->on_textinput=callback;}
bool bi_node_is_event_handler_available(BiNode* n);


#endif // __BINODE_H__
