#ifndef __BI_CORE_LAYER_H__
#define __BI_CORE_LAYER_H__

#include <bi/bi_gl.h>
#include <stdbool.h>
#include <bi/array.h>

typedef struct _BiNode BiNode;
typedef struct _BiTexture BiTexture;
typedef struct _BiShader BiShader;
typedef struct _BiLayerHeader BiLayerHeader;
typedef struct _BiLayer BiLayer;
typedef struct _BiLayerGroup BiLayerGroup;

enum BiLayerType {
  BI_LAYER_TYPE_LAYER,
  BI_LAYER_TYPE_LAYER_GROUP
};

struct _BiLayerHeader {
  int type;
};

struct _BiLayer {
  BiLayerHeader header;
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

struct _BiLayerGroup {
  BiLayerHeader header;
  Array layers;
  Array shaders;
};

extern void bi_layer_init(BiLayer* layer);

#endif
