#include <stdlib.h>
#include <string.h>
#include <bi/image.h>
#include <bi/bi_sdl.h>

void bi_image_rgba32_flip_vertical(int w, int h, uint8_t* pixels, uint8_t* flipped)
{
  const int pitch = 4*w;
  for(int y=0;y<h;y++){
    memcpy(&flipped[pitch*(h-1-y)], &pixels[pitch*y], pitch);
  }
}

void bi_image_rgba32_save_png(int w,int h,uint8_t* pixels, const char* filename)
{
  const int pitch = 4*w;
  SDL_ClearError();
  SDL_Surface* s = SDL_CreateRGBSurfaceWithFormatFrom(pixels,w,h,32,pitch,SDL_PIXELFORMAT_RGBA32);
  if(s==NULL){
    printf("SDL_CreateRGBSurfaceWithFormatFrom() failed.\n");
    const char* err = SDL_GetError();
    printf("Error: %s\n",err);
  }else{
    printf("SDL_Surface %d,%d\n",s->w,s->h);
  }
  IMG_SavePNG(s,filename);
  SDL_FreeSurface(s);
}
