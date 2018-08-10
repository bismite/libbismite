#include <bi/texture.h>
#include <bi/bi_sdl.h>
#include <GL/glew.h>
#include <stdio.h>
#include <stdlib.h>

static GLuint createTextureFromSurface(SDL_Surface *src, bool antialiase)
{
    SDL_Surface* img = src;

    // force change Format to SDL_PIXELFORMAT_ABGR8888
    if(src->format->format != SDL_PIXELFORMAT_ABGR8888)
    {
        img = SDL_CreateRGBSurfaceWithFormat(0, src->w, src->h, 32, SDL_PIXELFORMAT_ABGR8888);
        if (img == NULL) {
            SDL_Log("SDL_CreateRGBSurfaceWithFormat() failed: %s", SDL_GetError());
            return 0;
        }
        SDL_BlitSurface(src,NULL,img,NULL);
    }

    GLuint texture_id;
    glGenTextures(1, &texture_id);

    //
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glBindTexture(GL_TEXTURE_2D, texture_id);

    // SDL_PIXELFORMAT_ABGR8888 -> GL_RGBA
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img->w, img->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, img->pixels);

    //
    if(antialiase) {
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    }else{
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    }

    // XXX: Non-power-of-two textures must have a wrap mode of CLAMP_TO_EDGE
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // unbind
    glBindTexture(GL_TEXTURE_2D, 0);

    if(src->format->format != SDL_PIXELFORMAT_ABGR8888)
      SDL_FreeSurface(img);

    return texture_id;
}

bool bi_load_texture(const char* filename,BiTextureImage* texture_image, bool antialiase,int texture_unit)
{
    // XXX: ARGB? ABGR?
    // Desktop OpenGL: SDL_PIXELFORMAT_ARGB8888, invert R<->B
    // WebGL:          SDL_PIXELFORMAT_ABGR8888, correct.
    SDL_Surface *image = IMG_Load(filename);
    if(image == NULL) {
      printf("Error IMG_Load %s\n",filename);
      return false;
    }

    texture_image->texture_id = createTextureFromSurface(image,antialiase);
    texture_image->w = image->w;
    texture_image->h = image->h;
    texture_image->texture_unit = texture_unit;

    SDL_FreeSurface(image);

    return true;
}

void bi_set_texture_boundary(BiTexture* texture, uint16_t x, uint16_t y, uint16_t w, uint16_t h)
{
  texture->x = x;
  texture->y = y;
  texture->w = w;
  texture->h = h;
  // left, top, right, bottom
  double tw = texture->texture_image->w;
  double th = texture->texture_image->h;
  texture->boundary[0] = x / tw;
  texture->boundary[1] = y / th;
  texture->boundary[2] = (x+w) / tw;
  texture->boundary[3] = (y+h) / th;
}
