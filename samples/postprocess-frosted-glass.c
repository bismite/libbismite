#include "common.h"
#include "bi/canvas.h"

#define W 480
#define H 320

BiNode* glasses;
BiNode* glass_a;
BiNode* glass_b;
BiCanvas* canvas;

typedef struct {
  int vx;
  int vy;
} MoveData;

static void random_move(BiTimer* t,double dt)
{
  BiNode* nodes[3] = { glass_a, glass_b, NULL };
  for(int i=0; nodes[i] != NULL; i++ ){
    BiNode *n = nodes[i];
    MoveData* d = n->userdata;
    int x = n->x;
    int y = n->y;
    int w = n->w;
    int h = n->h;
    // move
    x += d->vx;
    y += d->vy;
    if(x < 0){   x = 0;   d->vx=-d->vx; }
    if(x+w > W){ x = W-w; d->vx=-d->vx; }
    if(y < 0){   y = 0;   d->vy=-d->vy; }
    if(y+h > H){ y = H-h; d->vy=-d->vy; }
    // backport
    bi_node_set_position(n,x,y);
  }
  // redraw
  bi_canvas_clear(canvas, 0,0,0, 0xff);
  bi_canvas_draw(canvas,glasses);
}

BiNode* make_glass(const char* name)
{
  BiNode* n = make_sprite_with_anchor(name,0,0);
  bi_node_set_position(n,10+rand()%(W-120),10+rand()%(H-120));
  MoveData *d = ALLOC(MoveData);
  d->vx = (rand()%2==0 ? 1 : -1);
  d->vy = (rand()%2==0 ? 1 : -1);
  n->userdata = d;
  return n;
}

int main(int argc, char* argv[])
{
  BiContext* context = make_context(__FILE__);
  // Shaders
  BiShader *pp_shader = create_shader_with_default_vertex_shader("assets/shaders/postprocess-frosted-glass.frag");
  // Main Layer
  BiLayer *layer = bi_layer_init(ALLOC(BiLayer));
  BiNode *bg = bi_layer_add_node(layer,make_bg("assets/check.png"));
  BiNode* face = make_sprite("assets/face01.png");
  bi_node_set_position(face,context->w/2,context->h/2);
  bi_node_add_node(bg,face);
  layer->textures[0] = bg->texture;
  layer->textures[1] = face->texture;
  // Canvas
  glasses = bi_node_init(ALLOC(BiNode));
  glass_a = make_glass("assets/glass-a.png");
  glass_b = make_glass("assets/glass-b.png");
  bi_node_add_node(glasses,glass_a);
  bi_node_add_node(glasses,glass_b);
  canvas = bi_canvas_init(ALLOC(BiCanvas),W,H);
  canvas->shader = &context->default_shader;
  canvas->textures[0] = glass_a->texture;
  canvas->textures[1] = glass_b->texture;
  bi_canvas_clear(canvas, 0,0,0, 0xff);
  bi_canvas_draw(canvas,glasses);
  BiTexture* canvas_tex = bi_canvas_to_texture(canvas, ALLOC(BiTexture));
  // PostProcess Layer
  BiLayer *pp_layer = bi_layer_init_as_postprocess(ALLOC(BiLayer));
  BiNode *root = bi_layer_add_node(pp_layer,bi_node_init(ALLOC(BiNode)));
  BiFramebuffer *fb = &context->layers.framebuffer;
  pp_layer->shader = pp_shader;
  bi_node_set_size(root,context->w,context->h);
  BiTexture *fb_tex = bi_texture_init_with_framebuffer(ALLOC(BiTexture),fb);
  bi_node_set_texture(root, fb_tex, 0,0,fb_tex->w,fb_tex->h);
  root->texture_flip_vertical = true;
  pp_layer->textures[0] = fb_tex;
  pp_layer->textures[1] = canvas_tex;
  // move glasses
  onupdate(root,random_move);
  // add layers
  bi_add_layer(context,layer);
  bi_add_layer(context,pp_layer);
  //
  bi_start_run_loop(context);
  return 0;
}
