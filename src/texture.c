#include <bi/texture.h>
#include <bi/bi_gl.h>
#include <bi/bi_sdl.h>
#include <bi/logger.h>

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

static bool _create_texture(BiTexture* texture, SDL_RWops* rwops, bool antialias)
{
    // XXX: ARGB? ABGR?
    // Desktop OpenGL: SDL_PIXELFORMAT_ARGB8888, invert R<->B
    // WebGL:          SDL_PIXELFORMAT_ABGR8888, correct.
    SDL_Surface *image = IMG_Load_RW( rwops, 1 );
    if(image == NULL) {
      LOG("Error IMG_Load\n");
      return false;
    }

    texture->texture_id = createTextureFromSurface(image,antialias);
    texture->w = image->w;
    texture->h = image->h;
    texture->_texture_unit = 0;

    SDL_FreeSurface(image);
    return true;
}

void bi_texture_init(BiTexture* texture)
{
  texture->texture_id = 0;
  texture->w = 0;
  texture->h = 0;
  texture->_texture_unit = 0;
}

bool bi_texture_load_from_memory(BiTexture* texture, void* buffer, size_t size, bool antialias)
{
    return _create_texture( texture, SDL_RWFromMem(buffer,size), antialias );
}

bool bi_texture_load_from_file(BiTexture* texture, const char* filename, bool antialias)
{
    return _create_texture( texture, SDL_RWFromFile(filename,"rb"), antialias );
}

void bi_texture_mapping_init(BiTextureMapping* texture_mapping)
{
  texture_mapping->texture = NULL;
  texture_mapping->x = 0;
  texture_mapping->y = 0;
  texture_mapping->w = 0;
  texture_mapping->h = 0;
  for(int i=0;i<4;i++) texture_mapping->boundary[i] = 0;
  texture_mapping->flip_vertical = false;
  texture_mapping->flip_horizontal = false;
}

void bi_texture_mapping_set_bound(BiTextureMapping* t, uint16_t x, uint16_t y, uint16_t w, uint16_t h)
{
  t->x = x;
  t->y = y;
  t->w = w;
  t->h = h;
  // left, top, right, bottom
  double tw = t->texture->w;
  double th = t->texture->h;
  t->boundary[0] = x / tw;
  t->boundary[1] = y / th;
  t->boundary[2] = (x+w) / tw;
  t->boundary[3] = (y+h) / th;
}
