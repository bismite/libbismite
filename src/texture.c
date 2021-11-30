#include <bi/texture.h>
#include <bi/bi_gl.h>
#include <bi/bi_sdl.h>
#include <bi/logger.h>
#include <bi/layer.h>

static GLuint create_texture_from_pixels(int w, int h, void*pixels, GLint internal_format, GLenum format, bool antialiase)
{
  GLuint texture_id;
  glGenTextures(1, &texture_id);

  //
  glBindTexture(GL_TEXTURE_2D, texture_id);
  glTexImage2D(GL_TEXTURE_2D, 0, internal_format, w, h, 0, format, GL_UNSIGNED_BYTE, pixels);

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

  return texture_id;
}

static bool get_format(SDL_PixelFormatEnum sdl_format, GLint* internal_format, GLenum* format)
{
  switch(sdl_format) {
    case SDL_PIXELFORMAT_ABGR8888:
    case SDL_PIXELFORMAT_BGR888:
      *internal_format = GL_RGBA;
      *format = GL_RGBA;
      break;
    case SDL_PIXELFORMAT_ARGB8888:
    case SDL_PIXELFORMAT_RGB888:
      *internal_format = GL_RGBA;
      *format = GL_BGRA_EXT;
      break;
    case SDL_PIXELFORMAT_RGB24:
      *internal_format = GL_RGB;
      *format = GL_RGB;
      break;
    default:
      return false;
  }
  return true;
}

static BiTexture* load_texture_from_image(BiTexture* texture, SDL_RWops* rwops, bool antialias)
{
  SDL_Surface *img = IMG_Load_RW( rwops, 1 );
  if(img == NULL) {
    LOG("IMG_Load_RW failed. Error:%s\n", IMG_GetError());
    return NULL;
  }

  GLint internal_format;
  GLenum format;
  if( get_format(img->format->format,&internal_format,&format) ) {
    texture->texture_id = create_texture_from_pixels(img->w,img->h,img->pixels,internal_format,format,antialias);
    texture->w = img->w;
    texture->h = img->h;
    texture->_texture_unit = 0;
  }else{
    SDL_Surface* tmp = SDL_ConvertSurfaceFormat(img, SDL_PIXELFORMAT_ABGR8888, 0);
    if (tmp ) {
      texture->texture_id = create_texture_from_pixels(tmp->w,tmp->h,tmp->pixels,GL_RGBA,GL_RGBA,antialias);
      texture->w = tmp->w;
      texture->h = tmp->h;
      texture->_texture_unit = 0;
      SDL_FreeSurface(tmp);
    }else{
      LOG("ConvertSurfaceFormat failed. Format:%s Error:%s", SDL_GetPixelFormatName(img->format->format), SDL_GetError() );
    }
  }

  SDL_FreeSurface(img);
  return texture;
}

BiTexture* bi_texture_init_with_file(BiTexture* texture, void* buffer, size_t size, bool antialias)
{
  return load_texture_from_image( texture, SDL_RWFromMem(buffer,size), antialias );
}

BiTexture* bi_texture_init_with_filename(BiTexture* texture, const char* filename, bool antialias)
{
  return load_texture_from_image( texture, SDL_RWFromFile(filename,"rb"), antialias );
}

void bi_texture_delete(BiTexture* texture)
{
  glDeleteTextures(1,&texture->texture_id);
  texture->texture_id = 0;
  texture->w = 0;
  texture->h = 0;
  texture->_texture_unit = 0;
}

BiTextureMapping* bi_texture_mapping_init(BiTextureMapping* texture_mapping,BiTexture* texture)
{
  texture_mapping->texture = texture;
  texture_mapping->x = 0;
  texture_mapping->y = 0;
  texture_mapping->w = 0;
  texture_mapping->h = 0;
  for(int i=0;i<4;i++) texture_mapping->boundary[i] = 0;
  if(texture){
    bi_texture_mapping_set_bound(texture_mapping,0,0,texture->w,texture->h);
  }
  texture_mapping->flip_vertical = false;
  texture_mapping->flip_horizontal = false;
  return texture_mapping;
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
