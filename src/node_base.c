#include <bi/node_base.h>
#include <stddef.h>
#include <stdlib.h>

void bi_node_base_init(BiNodeBase* node)
{
  node->z = 0;
  node->index = 0;
  node->interaction_enabled = true;
  node->time_scale = 1.0;
}
