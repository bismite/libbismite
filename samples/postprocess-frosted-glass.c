#include "common.h"

BiNode* glasses;
BiNode* glass_a;
BiNode* glass_b;
BiFramebufferNode* canvas;

typedef struct {
  int vx;
  int vy;
} MoveData;

static void random_move(BiContext* ctx,BiTimer* t,double dt)
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
  bi_framebuffer_node_draw(canvas,ctx);
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

BiFramebufferNode* make_canvas(BiContext* ctx)
{
  canvas = bi_framebuffer_node_init_with_size(ALLOC(BiFramebufferNode),W,H);
  // Nodes
  glasses = bi_node_init(ALLOC(BiNode));
  glass_a = make_glass("assets/glass-a.png");
  glass_b = make_glass("assets/glass-b.png");
  bi_node_add_node(glasses,glass_a);
  bi_node_add_node(glasses,glass_b);
  // Layer
  BiShaderNode *shader_node = bi_shader_node_init(ALLOC(BiShaderNode));
  bi_shader_node_add_node(shader_node,glasses);
  shader_node->textures[0] = glass_a->texture;
  shader_node->textures[1] = glass_b->texture;
  bi_framebuffer_node_add_shader_node(canvas,shader_node);
  // draw
  bi_framebuffer_node_draw(canvas,ctx);
  return canvas;
}


int main(int argc, char* argv[])
{
  BiContext* context = make_context(__FILE__);
  // Shaders
  BiShader *pp_shader = create_shader_with_default_vertex_shader("assets/shaders/postprocess-frosted-glass.frag");
  // Main Layer
  BiShaderNode *shader_node = bi_shader_node_init(ALLOC(BiShaderNode));
  BiNode *bg = bi_shader_node_add_node(shader_node,make_bg("assets/check.png"));
  BiNode* face = make_sprite("assets/face01.png");
  bi_node_set_position(face,context->w/2,context->h/2);
  bi_node_add_node(bg,face);
  shader_node->textures[0] = bg->texture;
  shader_node->textures[1] = face->texture;

  // Canvas
  make_canvas(context);
  BiTexture* canvas_tex = bi_texture_init_with_framebuffer(ALLOC(BiTexture),&canvas->framebuffer);
  // bi_framebuffer_save_png_image(&canvas->framebuffer,"postprocess-frosted-glass.png");

  // PostProcess Layer
  BiShaderNode *pp_shader_node = bi_shader_node_init_as_postprocess(ALLOC(BiShaderNode));
  BiNode *root = bi_shader_node_add_node(pp_shader_node,bi_node_init(ALLOC(BiNode)));
  BiFramebuffer *fb = &context->shader_nodes.framebuffer;
  pp_shader_node->shader = pp_shader;
  bi_node_set_size(root,context->w,context->h);
  BiTexture *fb_tex = bi_texture_init_with_framebuffer(ALLOC(BiTexture),fb);
  bi_node_set_texture(root, fb_tex, 0,0,fb_tex->w,fb_tex->h);
  root->texture_flip_vertical = true;
  pp_shader_node->textures[0] = fb_tex;
  pp_shader_node->textures[1] = canvas_tex;
  // move glasses
  onupdate(root,random_move);
  // add shader_nodes
  bi_add_shader_node(context,shader_node);
  bi_add_shader_node(context,pp_shader_node);
  //
  bi_start_run_loop(context);
  return 0;
}
