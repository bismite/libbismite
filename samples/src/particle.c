#include "common.h"

struct particle_data {
  float x;
  float y;
  float vx;
  float vy;
};

static void random_move(BiContext* c,BiTimer* t,double dt)
{
  BiNode *node = (BiNode*)t->node;
  struct particle_data *p = node->userdata;

  p->x += p->vx;
  p->y += p->vy;
  if(p->x < 0) p->x = 0;
  if(p->x > c->w) p->x = c->w;
  if(p->y < 0) p->y = 0;
  if(p->y > c->h) p->y = c->h;

  if( p->x == 0 || p->x == c->w) {
    p->vx = - p->vx;
  }
  if( p->y == 0 || p->y == c->h) {
    p->vy = - p->vy;
  }

  bi_node_set_position(node, p->x, p->y );
}

static BiNode* create_particle(BiContext* c,BiTexture *tex)
{
  BiNode* node = malloc(sizeof(BiNode));
  bi_node_init(node);
  bi_node_set_texture(node,tex,0,0,tex->w,tex->h);
  bi_node_set_anchor(node,0.5,0.5);

  // position, scale, size
  bi_node_set_position(node, rand() % c->w, rand() % c->h );
  bi_node_set_size(node,tex->w,tex->h);
  float scale = 0.1 + rand()%200 / 100.0;
  bi_node_set_scale(node,scale,scale);

  // color
  bi_set_color(node->color, rand()%0xff, rand()%0xff, rand()%0xff, 0xff);

  // on update callback
  onupdate(node,random_move);

  // velocity
  struct particle_data* p = malloc(sizeof(struct particle_data));
  node->userdata = p;
  p->x = node->_x;
  p->y = node->_y;
  p->vx = (rand()%100/100.0) * 0.4 * (rand()%2==0 ? 1 : -1);
  p->vy = (rand()%100/100.0) * 0.4 * (rand()%2==0 ? 1 : -1);

  return node;
}

int main(int argc, char* argv[])
{
  srand( bi_get_now() );
  BiContext* context = malloc(sizeof(BiContext));
  bi_init_context(context, 480, 320, 0, true, __FILE__);
  print_info(context);

  // texture
  BiTexture* ball_texture = MAKE_TEXTURE("assets/ball.png");

  // layer
  BiLayer *layer = malloc(sizeof(BiLayer));
  bi_layer_init(layer);
  layer->root = make_sprite_with_anchor("assets/map.png",0,0);;
  bi_add_layer(context,layer);
  layer->textures[0] = layer->root->_texture;
  layer->textures[1] = ball_texture;
  // additive blending
  layer->blend_factor.src = GL_SRC_ALPHA;
  layer->blend_factor.dst = GL_ONE;

  bi_set_blend_factor(&layer->blend_factor,GL_SRC_ALPHA,GL_ONE,GL_SRC_ALPHA,GL_ONE);

  // particles
  for(uint64_t i=0; i< 1024 ; i++){
    bi_node_add_node(layer->root, create_particle(context,ball_texture));
  }

  //
  // fps layer
  //
  BiFontAtlas *font = load_font();
  add_fps_layer(context,font);

  bi_start_run_loop(context);
  return 0;
}
