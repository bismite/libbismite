#ifndef __BI_CORE_LAYER_H__
#define __BI_CORE_LAYER_H__

#include <GL/glew.h>
#include <stdbool.h>

struct _BiNode;
typedef struct _BiNode BiNode;

struct _BiTextureImage;
typedef struct _BiTextureImage BiTextureImage;

struct _BiLayer;
typedef struct _BiLayer BiLayer;

struct _BiLayer {
  GLfloat camera_x;
  GLfloat camera_y;
  bool projection_centering;
  BiNode* root;
  GLenum blend_src;
  GLenum blend_dst;
  BiTextureImage* textures[8];
};

extern void bi_layer_init(BiLayer* layer);

#endif
