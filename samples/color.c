#include "common.h"
#include <math.h>

// h:[0-1] s:[0-1] v:[0-1]
void hsv_to_rgb(double h, double s, double v, uint8_t *r, uint8_t *g, uint8_t *b)
{
  static const double unit = 1.0 / 6.0;
  double c = 0.0, m = 0.0, x = 0.0;
  double rr=0.0, gg=0.0, bb=0.0;
  if(h>1.0) h = 1.0-fmod(h,1.0);
  if(h<0.0) h = 1.0+fmod(h,1.0);
  c = v * s;
  x = c * (1.0 - fabs(fmod(h/unit, 2) - 1.0));
  m = v - c;
  if (h >= 0.0 && h < unit) {
    rr=c+m; gg=x+m; bb=m;
  } else if ( h >= unit && h < 2*unit ) {
    rr=x+m; gg=c+m; bb=m;
  } else if ( h >= 2*unit && h < 3*unit ) {
    rr=m; gg=c+m; bb=x+m;
  } else if ( h >= 3*unit && h < 4*unit ) {
    rr=m; gg=x+m; bb=c+m;
  } else if ( h >= 4*unit && h < 5*unit ) {
    rr=x+m; gg=m; bb=c+m;
  } else if ( h >= 5*unit && h < 6*unit ) {
    rr=c+m; gg=m; bb=x+m;
  }
  *r = (uint8_t)(rr*0xff); *g = (uint8_t)(gg*0xff); *b =(uint8_t)(bb*0xff) ;
}

static void magick_color_half_tint(BiContext* ctx,BiTimer* t,double dt)
{
  static double h=0;
  uint8_t r=0,g=0,b=0;
  BiNode *node = t->userdata;
  h+=0.02;
  if(h>1.0) h = fmod(h,1.0);
  hsv_to_rgb(h,1,1, &r,&g,&b);
  node->tint = RGBA(r,g,b,128);
}

static void magick_color_modulate(BiContext* ctx,BiTimer* t,double dt)
{
  static double h=0;
  uint8_t r=0,g=0,b=0;
  BiNode *node = t->userdata;
  h+=0.01;
  if(h>1.0) h = fmod(h,1.0);
  hsv_to_rgb(h,1,1, &r,&g,&b);
  node->color = RGBA(r,g,b,0xff);
}

static void magick_color_full_tint(BiContext* ctx,BiTimer* t,double dt)
{
  static double h=0;
  uint8_t r=0,g=0,b=0;
  BiNode *node = t->userdata;
  h+=0.01;
  if(h>1.0) h = fmod(h,1.0);
  hsv_to_rgb(h,1,1, &r,&g,&b);
  node->tint = RGBA(r,g,b,0xff);
}

int main(int argc, char* argv[])
{
  BiContext* context = make_context(__FILE__);

  // Texture
  BiTexture* tex = MAKE_TEXTURE("assets/mushroom.png");

  // shader_node
  BiShaderNode *shader_node = bi_shader_node_init(ALLOC(BiShaderNode));
  bi_add_shader_node(context,shader_node);
  BiNode* background = bi_shader_node_add_node(shader_node,make_bg("assets/map.png"));
  shader_node->textures[0] = background->texture;
  shader_node->textures[1] = tex;

  // colors
  int colors[21][2][4] = {
    { {0,0,0,0}, {0xff,0xff,0xff,0xff} }, // Magick
    { {0,0,0,0}, {0xff,0xff,0xff,0xff} }, // Magick2
    { {0,0,0,0}, {0xff,0xff,0xff,0xff} }, // Magick3
    { {0,0,0,0}, {0xff,0xff,0xff,0xff} }, // Normal
    { {0,0,0,0}, {0xff,0xff,0xff,128} }, // 1/2 Transparent
    { {0,0,0,0}, {0xff,0xff,0xff, 64} }, // 3/4 Transparent
    { {0,0,0,0}, {0xff,0xff,0xff,  0} }, // Transparent

    { {0xff,0,0,128}, {0xff,0xff,0xff,0xff} }, // Tint Red
    { {0,0xff,0,128}, {0xff,0xff,0xff,0xff} }, // Tint Green
    { {0,0,0xff,128}, {0xff,0xff,0xff,0xff} }, // Tint Blue
    { {0,0,0,128},    {0xff,0xff,0xff,0xff} }, // Tint Half Black
    { {0,0,0,0xff},    {0xff,0xff,0xff,0xff} }, // Tint Full Black
    { {0xff,0xff,0xff,128},    {0xff,0xff,0xff,0xff} }, // Tint Half White
    { {0xff,0xff,0xff,0xff},    {0xff,0xff,0xff,0xff} }, // Tint Full White

    { {0,0,0,0}, {0xff,128,128,0xff} }, // Modulate Red
    { {0,0,0,0}, {128,0xff,128,0xff} }, // Modulate Green
    { {0,0,0,0}, {128,128,0xff,0xff} }, // Modulate Blue
    { {0,0,0,0}, {0xff,0xff,0xff,0xff} },// Modulate 1 (Normal)
    { {0,0,0,0}, {128,128,128,0xff} },  // Modulate 0.5
    { {0,0,0,0}, {64,64,64,0xff} },     // Modulate 0.25
    { {0,0,0,0}, {0,0,0,0xff} },        // Modulate 0
  };

  for(int i=0; i<21; i++) {
    int *a = colors[i][0];
    int *b = colors[i][1];
    BiNode* n = make_sprite_from_texture(tex);
    int x = i%7;
    int y = i/7;
    bi_node_set_position(n,32+14+x*64,96+y*64);
    n->tint = RGBA(a[0], a[1], a[2], a[3]);
    n->color = RGBA(b[0], b[1], b[2], b[3]);
    bi_node_add_node(background,n);
    // Magick color
    if(i==0) { onupdate(n,magick_color_half_tint); }
    if(i==1) { onupdate(n,magick_color_modulate); }
    if(i==2) { onupdate(n,magick_color_full_tint); }
  }

  bi_start_run_loop(context);
  return 0;
}
