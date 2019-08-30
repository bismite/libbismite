#include <bi/texture.h>
#include <bi/bi_sdl.h>
#include <bi/logger.h>
#include <GL/glew.h>

static SDL_Surface* create_surface_abgr8888(int w, int h)
{
#if SDL_VERSION_ATLEAST(2,0,5)
  return SDL_CreateRGBSurfaceWithFormat(0, w, h, 32, SDL_PIXELFORMAT_ABGR8888);
#else
  Uint32 rmask, gmask, bmask, amask;
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
  rmask = 0xff000000;
  gmask = 0x00ff0000;
  bmask = 0x0000ff00;
  amask = 0x000000ff;
#else
  rmask = 0x000000ff;
  gmask = 0x0000ff00;
  bmask = 0x00ff0000;
  amask = 0xff000000;
#endif
  return SDL_CreateRGBSurface(0, w, h, 32, rmask, gmask, bmask, amask);
#endif
}

static GLuint createTextureFromSurface(SDL_Surface *src, bool antialiase)
{
    SDL_Surface* img = src;

    // force change Format to SDL_PIXELFORMAT_ABGR8888
    if(src->format->format != SDL_PIXELFORMAT_ABGR8888)
    {
        img = create_surface_abgr8888(src->w, src->h);
        if (img == NULL) {
            LOG("SDL_CreateRGBSurfaceWithFormat() failed: %s", SDL_GetError());
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

static bool _create_texture(BiTextureImage* texture_image, SDL_RWops* rwops, bool antialias)
{
    // XXX: ARGB? ABGR?
    // Desktop OpenGL: SDL_PIXELFORMAT_ARGB8888, invert R<->B
    // WebGL:          SDL_PIXELFORMAT_ABGR8888, correct.
    SDL_Surface *image = IMG_Load_RW( rwops, 1 );
    if(image == NULL) {
      LOG("Error IMG_Load\n");
      return false;
    }

    texture_image->texture_id = createTextureFromSurface(image,antialias);
    texture_image->w = image->w;
    texture_image->h = image->h;
    texture_image->_texture_unit = 0;

    SDL_FreeSurface(image);
    return true;
}

bool bi_create_texture(void* buffer, size_t size, BiTextureImage* texture_image, bool antialias)
{
    return _create_texture( texture_image, SDL_RWFromMem(buffer,size), antialias );
}

bool bi_load_texture(const char* filename, BiTextureImage* texture_image, bool antialias)
{
    return _create_texture( texture_image, SDL_RWFromFile(filename,"rb"), antialias );
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
