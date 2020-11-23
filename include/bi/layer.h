#ifndef __BI_CORE_LAYER_H__
#define __BI_CORE_LAYER_H__

#include <bi/bi_gl.h>
#include <stdbool.h>

struct _BiNode;
typedef struct _BiNode BiNode;

struct _BiTexture;
typedef struct _BiTexture BiTexture;

struct _BiLayer;
typedef struct _BiLayer BiLayer;

struct _BiShader;
typedef struct _BiShader BiShader;

struct _BiLayer {
  GLfloat camera_x;
  GLfloat camera_y;
  bool projection_centering;
  BiNode* root;
  GLenum blend_src;
  GLenum blend_dst;
  BiTexture* textures[8];
  int z_order;
  int index;
  BiShader *shader;
  GLfloat optional_shader_attributes[4];
};

extern void bi_layer_init(BiLayer* layer);

#endif
