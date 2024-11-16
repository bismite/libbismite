#ifndef __BISMITE_SHADER_NODE_H__
#define __BISMITE_SHADER_NODE_H__

#include <stdbool.h>
#include <bi/bi_gl.h>
#include <bi/node_base.h>
#include <bi/array.h>

#define BI_LAYER_MAX_TEXTURES 16

typedef struct _BiTexture BiTexture;
typedef struct _BiNode BiNode;
typedef struct _BiShader BiShader;
typedef struct _BiShaderNode BiShaderNode;

struct _BiShaderNode {
  BI_NODE_HEADER;
  void* _render_function_;
  BiBlendFactor blend_factor;
  GLfloat camera_x;
  GLfloat camera_y;
  BiTexture* textures[BI_LAYER_MAX_TEXTURES];
  BiShader *shader;
  GLfloat shader_extra_data[16];
};

extern BiShaderNode* bi_shader_node_init(BiShaderNode* shader_node);
extern BiShaderNode* bi_shader_node_init_as_postprocess(BiShaderNode* shader_node);
static inline int bi_shader_node_get_z_order(BiShaderNode* shader_node){ return shader_node->z; }
static inline void bi_shader_node_set_z_order(BiShaderNode* shader_node,int z){ shader_node->z = z; }
// Tree
extern BiShaderNode* bi_shader_node_remove_from_parent(BiShaderNode* shader_node);
extern BiNode* bi_shader_node_add_node(BiShaderNode *shader_node,BiNode* node);
extern BiNode* bi_shader_node_remove_node(BiShaderNode *shader_node,BiNode* node);
// Timer
extern BiTimer* bi_shader_node_add_timer(BiShaderNode* shader_node,BiTimer* timer);
extern BiTimer* bi_shader_node_remove_timer(BiShaderNode* shader_node,BiTimer* timer);

#endif // __BISMITE_SHADER_NODE_H__
