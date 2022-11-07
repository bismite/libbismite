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
  BiContext* context = bi_init_context(ALLOC(BiContext), 480, 320, 0, false, __FILE__);
  print_info(context);

  // background
  BiNode *bg = make_sprite("assets/check.png");
  bi_node_set_position(bg,context->w/2,context->h/2);

  // face sprite
  BiNode *face = make_sprite("assets/face01.png");
  bi_node_set_scale(face,2.0,2.0);
  bi_node_add_node(bg,face);

  // shade
  BiNode* shade = ALLOC(BiNode);
  bi_node_init(shade);
  bi_node_set_anchor(shade,0.5,0.5);
  bi_node_set_size(shade,context->w,context->h);
  bi_set_color( shade->color, 0,0,0, 128);
  bi_node_add_node(bg,shade);

  // layer
  BiLayer *layer = malloc(sizeof(BiLayer));
  bi_layer_init(layer);
  layer->root = bg;
  layer->textures[0] = bg->texture;
  layer->textures[1] = face->texture;

  bi_add_layer(context,layer);

  //
  // spotlight layer
  //
  BiTexture *texture = malloc(sizeof(BiTexture));
  bi_texture_init_with_filename(texture,"assets/circle256.png",true);
  // sprite
  BiNode *spotlight = create_spotlight(texture,context->w/3,context->h/2);
  BiNode *spotlight2 = create_spotlight(texture,context->w/3*2,context->h/2);
  // layer
  BiLayer *spotlight_layer = malloc(sizeof(BiLayer));
  bi_layer_init(spotlight_layer);
  spotlight_layer->root = malloc(sizeof(BiNode));
  bi_node_init(spotlight_layer->root);
  bi_node_add_node(spotlight_layer->root,spotlight);
  bi_node_add_node(spotlight_layer->root,spotlight2);
  // blend setting
  spotlight_layer->blend_factor.src = GL_DST_COLOR;
  spotlight_layer->blend_factor.dst = GL_ONE;
  // textures
  spotlight_layer->textures[0] = spotlight->texture;
  spotlight_layer->textures[1] = spotlight2->texture;

  bi_add_layer(context,spotlight_layer);

  //
  // fps layer
  //
  BiFontAtlas *font = load_font();
  add_fps_layer(context,font);

  bi_start_run_loop(context);
  return 0;
}
