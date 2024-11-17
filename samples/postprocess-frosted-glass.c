#include "common.h"

BiNode* glass_a;
BiNode* glass_b;

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

BiNode* make_mask_framebuffer(BiContext* ctx)
{
  // Framebuffer
  BiNode* node = bi_node_init(ALLOC(BiNode));
  bi_node_set_size(node,W,H);
  node->framebuffer = bi_framebuffer_init(ALLOC(BiFramebuffer),W,H);
  BiTexture* tex = bi_texture_init_with_framebuffer(ALLOC(BiTexture), node->framebuffer);
  bi_node_set_texture(node,tex,0,0,tex->w,tex->h);
  node->texture_flip_vertical = true;
  // Shader
  BiShaderNode *snode = bi_shader_node_init(ALLOC(BiShaderNode));
  glass_a = bi_node_add_node(snode,make_glass("assets/glass-a.png"));
  glass_b = bi_node_add_node(snode,make_glass("assets/glass-b.png"));
  snode->textures[0] = bi_node_get_texture(glass_a);
  snode->textures[1] = bi_node_get_texture(glass_b);
  bi_node_add_node(node,snode);
  // move glasses
  onupdate(node,random_move);
  return node;
}

int main(int argc, char* argv[])
{
  BiContext* context = make_context(__FILE__);

  // Main Framebuffer
  BiNode* main = bi_node_init(ALLOC(BiNode));
  bi_node_set_size(main,W,H);
  main->framebuffer = bi_framebuffer_init(ALLOC(BiFramebuffer),W,H);
  BiTexture* tex = bi_texture_init_with_framebuffer(ALLOC(BiTexture), main->framebuffer);
  bi_node_set_texture(main,tex,0,0,tex->w,tex->h);
  main->texture_flip_vertical = true;
  // Main Shader
  BiShaderNode *snode = bi_shader_node_init(ALLOC(BiShaderNode));
  BiNode *bg = bi_node_add_node(snode, make_bg("assets/check.png"));
  BiNode* face = bi_node_add_node(snode, make_sprite("assets/face01.png"));
  bi_node_set_position(face,W/2,H/2);
  snode->textures[0] = bi_node_get_texture(bg);
  snode->textures[1] = bi_node_get_texture(face);
  bi_node_add_node(main,snode);

  // Mask Framebuffer
  BiNode* mask = make_mask_framebuffer(context);

  // Post Process
  snode = bi_shader_node_init(ALLOC(BiShaderNode));
  snode->shader = create_shader_with_default_vertex_shader("assets/shaders/postprocess-frosted-glass.frag");
  bi_node_add_node(snode,main);
  bi_node_add_node(snode,mask);
  snode->textures[0] = bi_node_get_texture(main);
  snode->textures[1] = bi_node_get_texture(mask);
  bi_node_add_node(&context->default_framebuffer_node,snode);

  //
  bi_start_run_loop(context);
  return 0;
}
