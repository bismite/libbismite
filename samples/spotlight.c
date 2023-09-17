#include "common.h"
#include <stdlib.h>

static void spin(BiTimer* t,double dt)
{
  BiNode *node = t->userdata;
  bi_node_set_angle(node, node->angle + 0.01);
}

static BiNode* create_spotlight(BiTexture* texture,float x, float y)
{
  // sprite
  BiNode* sprite = make_sprite_from_texture(texture);
  bi_node_set_position(sprite,x,y);
  // spin spotlight
  onupdate(sprite,spin);
  return sprite;
}

int main(int argc,char* argv[])
{
  BiContext* context = make_context(__FILE__);
  // layer
  BiLayer *layer = bi_layer_init(ALLOC(BiLayer));
  BiNode *bg = set_texture(&layer->root,"assets/check.png");
  bi_add_layer(context,layer);
  // face sprite
  BiNode *face = make_sprite("assets/face01.png");
  bi_node_set_scale(face,2.0,2.0);
  bi_node_set_position(face,W/2,H/2);
  bi_node_add_node(bg,face);
  // shade
  BiNode* shade = bi_node_init(ALLOC(BiNode));
  bi_node_set_size(shade,context->w,context->h);
  shade->color_modulate = RGBA(0,0,0,128);
  bi_node_add_node(bg,shade);
  // set textures
  layer->textures[0] = bg->texture;
  layer->textures[1] = face->texture;
  // spotlight sprite
  BiTexture *texture = bi_texture_init_with_filename(ALLOC(BiTexture),"assets/circle256.png",true);
  BiNode *spotlight = create_spotlight(texture,context->w/3,context->h/2);
  BiNode *spotlight2 = create_spotlight(texture,context->w/3*2,context->h/2);
  // spotlight layer
  BiLayer *spotlight_layer = bi_layer_init(ALLOC(BiLayer));
  bi_add_layer(context,spotlight_layer);
  bi_node_add_node(&spotlight_layer->root,spotlight);
  bi_node_add_node(&spotlight_layer->root,spotlight2);
  spotlight_layer->blend_factor.src = GL_DST_COLOR;
  spotlight_layer->blend_factor.dst = GL_ONE;
  // set textures
  spotlight_layer->textures[0] = spotlight->texture;
  spotlight_layer->textures[1] = spotlight2->texture;
  //
  bi_start_run_loop(context);
  return 0;
}
