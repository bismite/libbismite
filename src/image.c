#include <stdlib.h>
#include <string.h>
#include <bi/image.h>
#include <bi/bi_sdl.h>

void bi_image_rgba32_flip_vertical(int w, int h, uint8_t* pixels)
{
  const int pitch = 4*w;
  uint8_t* tmp = malloc(pitch);
  for(int y=0;y<h/2;y++){
    memcpy(tmp, &pixels[pitch*y], pitch);
    memcpy(&pixels[pitch*y], &pixels[pitch*(h-y)], pitch);
    memcpy(&pixels[pitch*(h-y)], tmp, pitch);
  }
  free(tmp);
}

void bi_image_rgba32_save(int w,int h,uint8_t* pixels, const char* filename)
{
  const int pitch = 4*w;
  SDL_Surface* s = SDL_CreateRGBSurfaceWithFormatFrom(pixels,w,h,32,pitch,SDL_PIXELFORMAT_RGBA32);
  IMG_SavePNG(s,filename);
  SDL_FreeSurface(s);
}
