#ifndef __BI_CORE_LAYER_H__
#define __BI_CORE_LAYER_H__

#include <GL/glew.h>

struct _BiNode;
typedef struct _BiNode BiNode;

struct _BiLayer {
  GLfloat camera_x;
  GLfloat camera_y;
  bool projection_centering;
  BiNode* root;
  GLenum blend_src;
  GLenum blend_dst;
};

#endif
