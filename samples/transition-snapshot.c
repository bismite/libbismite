#include "common.h"
#include <bi/canvas.h>

#define TRANSITION_SPEED 0.002;
BiLayerGroup *group_0;
BiLayerGroup *group_1;

typedef struct {
  BiLayerGroup *group0;
  BiLayerGroup *group1;
  BiTexture group1_framebuffer_texture;
  BiCanvas snapshot_canvas;
  BiTexture snapshot_texture;
} TransitionData;

static void on_update_transition(BiTimer* t,double dt)
{
  BiLayer *transition_layer = t->userdata;
  transition_layer->shader_extra_data[0] += dt * TRANSITION_SPEED
  if( transition_layer->shader_extra_data[0] > 1.0 ) {
    transition_layer->shader_extra_data[0] = 1.0;
    bi_layer_remove_from_parent(transition_layer);
  }
}

static void init_transition(BiContext* context,
                            BiLayer *transition_layer,
                            BiShader* postprocess_shader,
                            BiLayerGroup* group0,
                            BiLayerGroup* group1 )
{
  BiTexture* tex = bi_texture_init_with_framebuffer(ALLOC(BiTexture),&group0->framebuffer);
  TransitionData *dat = ALLOC(TransitionData);
  dat->group0 = group0;
  dat->group1 = group1;

  BiCanvas* snapshot = &dat->snapshot_canvas;
  bi_canvas_init(snapshot, tex->w, tex->h);
  snapshot->shader = &context->default_shader;
  snapshot->textures[0] = tex;
  BiNode n;
  bi_node_init(&n);
  bi_node_set_size(&n,tex->w,tex->h);
  bi_node_set_texture(&n,tex,0,0,tex->w,tex->h);
  bi_canvas_clear(snapshot,0,0,0,0);
  bi_canvas_draw(snapshot,&n);
  bi_texture_init_with_framebuffer(&dat->snapshot_texture,&snapshot->framebuffer);
  bi_texture_init_with_framebuffer(&dat->group1_framebuffer_texture,&group_1->framebuffer);
  //
  transition_layer->shader = postprocess_shader;
  transition_layer->userdata = dat;
  transition_layer->textures[0] = &dat->snapshot_texture;
  transition_layer->textures[1] = &dat->group1_framebuffer_texture;
  transition_layer->root = bi_node_init(ALLOC(BiNode));
  bi_node_set_size(transition_layer->root,context->w,context->h);
  BiTimer *t = bi_timer_init(ALLOC(BiTimer),on_update_transition,0,-1,transition_layer);
  bi_layer_add_timer(transition_layer,t);
}

static void on_update_rotate(BiTimer* t,double dt)
{
  BiNode* n = t->userdata;
  bi_node_set_angle(n,n->angle+dt*0.01);
}

static bool on_click(BiContext* context,BiNode* n, int x, int y, int button, bool pressed)
{
  if(pressed) return true;

  // remove group_0
  bi_layer_group_remove_layer_group(&context->layers,group_0);
  // add group_1
  bi_layer_group_add_layer_group(&context->layers,group_1);
  // Transition
  BiShader* shader = create_shader_with_default_vertex_shader("assets/shaders/transition-stairs.frag");
  BiLayer *transition_layer = bi_layer_init_as_postprocess(ALLOC(BiLayer));
  init_transition(context,transition_layer,shader,group_0,group_1);
  bi_add_layer(context,transition_layer);

  return true;
}

static BiLayerGroup* make_group_0(BiContext* context)
{
  BiNode* bg = make_sprite("assets/check.png");
  bi_node_set_position(bg,context->w/2,context->h/2);
  bi_node_set_on_click(bg, on_click);
  BiNode* sprite = make_sprite("assets/face01.png");
  bi_node_add_node(bg,sprite);
  onupdate(sprite,on_update_rotate);
  bi_node_set_on_click(bg, on_click);
  BiLayer *layer = bi_layer_init(ALLOC(BiLayer));
  layer->root = bg;
  layer->textures[1] = bg->texture;
  layer->textures[0] = sprite->texture;
  BiLayerGroup* lg = bi_layer_group_init(ALLOC(BiLayerGroup));
  bi_layer_group_add_layer(lg,layer);
  return lg;
}

static BiLayerGroup* make_group_1(BiContext* context)
{
  BiNode* bg = make_sprite("assets/map.png");
  bi_node_set_position(bg,context->w/2,context->h/2);
  BiNode* sprite = make_sprite("assets/mushroom.png");
  bi_node_add_node(bg,sprite);
  onupdate(sprite,on_update_rotate);
  BiLayer *layer = bi_layer_init(ALLOC(BiLayer));
  layer->root = bg;
  layer->textures[1] = bg->texture;
  layer->textures[0] = sprite->texture;
  BiLayerGroup* lg = bi_layer_group_init(ALLOC(BiLayerGroup));
  bi_layer_group_add_layer(lg,layer);
  return lg;
}

int main(int argc, char* argv[])
{
  BiContext* context = bi_init_context(ALLOC(BiContext), 480, 320, 0, true, __FILE__);

  group_0 = make_group_0(context);
  group_1 = make_group_1(context);
  bi_layer_group_add_layer_group(&context->layers, group_0);

  // FPS
  BiFontAtlas *font = load_font();
  add_fps_layer(context,font);

  bi_start_run_loop(context);
  return 0;
}
