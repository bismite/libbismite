#ifndef __BISMITE_SHADER_NODE_H__
#define __BISMITE_SHADER_NODE_H__

#include <stdbool.h>
#include <bi/bi_gl.h>
#include <bi/node_base.h>
#include <bi/shader.h>

typedef struct _BiTexture BiTexture;
typedef struct _BiNode BiNode;
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
static inline int bi_shader_node_get_z_order(BiShaderNode* shader_node){ return shader_node->z; }
static inline void bi_shader_node_set_z_order(BiShaderNode* shader_node,int z){ shader_node->z = z; }

// Timer
extern BiTimer* bi_shader_node_add_timer(BiShaderNode* shader_node,BiTimer* timer);
extern BiTimer* bi_shader_node_remove_timer(BiShaderNode* shader_node,BiTimer* timer);

#endif // __BISMITE_SHADER_NODE_H__
