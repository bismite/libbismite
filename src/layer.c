#include <bi/layer.h>
#include <bi/node.h>

void bi_layer_init(BiLayer* layer)
{
  layer->root = NULL;
  layer->camera_x = 0;
  layer->camera_y = 0;
  layer->projection_centering = false;
  layer->blend_src = GL_SRC_ALPHA;
  layer->blend_dst = GL_ONE_MINUS_SRC_ALPHA;
  for(int i=0;i<8;i++) {
    layer->textures[i] = NULL;
  }
}
