#ifndef __BI_SHADER_NODE_H__
#define __BI_SHADER_NODE_H__

#include <stdbool.h>
#include <bi/bi_gl.h>
#include <bi/node_base.h>
#include <bi/shader.h>

typedef struct _BiTexture BiTexture;
typedef struct _BiShaderNode BiShaderNode;

struct _BiShaderNode {
  BI_NODE_HEADER;
  BiBlendFactor blend_factor;
  GLfloat camera_x;
  GLfloat camera_y;
  BiTexture* textures[BI_SHADER_MAX_TEXTURES];
  BiShader *shader;
  GLfloat shader_extra_data[16];
};

extern BiShaderNode* bi_shader_node_init(BiShaderNode* shader_node);

#endif // __BI_SHADER_NODE_H__
