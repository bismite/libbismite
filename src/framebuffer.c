#include <bi/layer.h>
#include <bi/node.h>
#include <bi/bi_sdl.h>

BiFramebuffer* bi_framebuffer_init(BiFramebuffer *fb,int w,int h)
{
  fb->w = w;
  fb->h = h;
  glGenFramebuffers(1, &fb->framebuffer_id);
  glBindFramebuffer(GL_FRAMEBUFFER, fb->framebuffer_id);
  glGenTextures(1, &fb->texture_id);
  glBindTexture(GL_TEXTURE_2D, fb->texture_id);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w,h, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fb->texture_id, 0);
  // unbind
  glBindTexture(GL_TEXTURE_2D, 0);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  return fb;
}

void bi_framebuffer_save_png_image(BiFramebuffer *fb,const char *filename)
{
  const int w = fb->w;
  const int h = fb->h;
  const int pitch = 4*w;
  uint8_t* pixels = malloc(pitch*h);
  glBindFramebuffer(GL_FRAMEBUFFER,fb->framebuffer_id);
  glReadPixels(0,0,fb->w,fb->h,GL_RGBA,GL_UNSIGNED_BYTE,pixels);
  glBindFramebuffer(GL_FRAMEBUFFER,0);
  SDL_Surface* s = SDL_CreateRGBSurfaceWithFormatFrom(pixels,w,h,32,pitch,SDL_PIXELFORMAT_RGBA32);
  IMG_SavePNG(s,filename);
  SDL_FreeSurface(s);
  free(pixels);
}
