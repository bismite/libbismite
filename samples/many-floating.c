#include "common.h"

#define NUM_OF_NODES 5000

struct particle_data {
  float x;
  float y;
  float vx;
  float vy;
};

static void random_move(BiContext* ctx,BiTimer* t,double dt)
{
  BiNode *node = t->userdata;
  struct particle_data *p = node->userdata;
  p->x += p->vx;
  p->y += p->vy;
  if(p->x < 0) p->x = 0;
  if(p->x > W) p->x = W;
  if(p->y < 0) p->y = 0;
  if(p->y > H) p->y = H;
  if( p->x == 0 || p->x == W) { p->vx = - p->vx; }
  if( p->y == 0 || p->y == H) { p->vy = - p->vy; }
  bi_node_set_position(node, p->x, p->y );
}

static BiNode* create_particle(BiContext* c,BiTexture *tex)
{
  BiNode* node = bi_node_init(ALLOC(BiNode));
  bi_node_set_texture(node,tex,0,0,tex->w,tex->h);
  bi_node_set_anchor(node,0.5,0.5);
  bi_node_set_position(node, rand() % c->w, rand() % c->h );
  bi_node_set_size(node,tex->w,tex->h);
  float scale = 0.1 + rand()%200 / 100.0;
  bi_node_set_scale(node,scale,scale);
  node->color = RGBA(rand()%0xff, rand()%0xff, rand()%0xff, 64);
  // on update callback
  onupdate(node,random_move);
  // velocity
  struct particle_data* p = malloc(sizeof(struct particle_data));
  node->userdata = p;
  p->x = node->x;
  p->y = node->y;
  p->vx = (rand()%100/100.0) * 0.4 * (rand()%2==0 ? 1 : -1);
  p->vy = (rand()%100/100.0) * 0.4 * (rand()%2==0 ? 1 : -1);
  return node;
}

int main(int argc, char* argv[])
{
  BiContext* context = make_context(__FILE__);
  // texture
  BiTexture* ball_texture = MAKE_TEXTURE("assets/ball.png");
  // shader_node
  BiShaderNode *shader_node = bi_shader_node_init(ALLOC(BiShaderNode));
  BiNode* root = bi_shader_node_add_node(shader_node,make_bg("assets/map.png"));
  bi_add_shader_node(context,shader_node);
  shader_node->textures[0] = root->texture;
  shader_node->textures[1] = ball_texture;
  // additive blending
  shader_node->blend_factor = BI_BLEND_FACTOR_ADDTIVE;
  // particles
  for(uint64_t i=0; i< NUM_OF_NODES ; i++){
    bi_node_add_node(root, create_particle(context,ball_texture));
  }
  //
  bi_start_run_loop(context);
  return 0;
}
