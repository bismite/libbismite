#include <bi/texture.h>
#include <bi/bi_gl.h>
#include <bi/bi_sdl.h>
#include <bi/logger.h>
#include <bi/layer.h>

extern GLuint bi_texture_convert_to_premultiplied_alpha(GLuint texture_id,GLint tex_format,int w,int h);

static GLuint create_texture_from_pixels(int w, int h, void*pixels,
                                         GLint tex_format, GLenum img_format, bool antialiase)
{
  GLuint texture_id;
  glGenTextures(1, &texture_id);

  //
  glBindTexture(GL_TEXTURE_2D, texture_id);
  glTexImage2D(GL_TEXTURE_2D, 0, tex_format, w, h, 0, img_format, GL_UNSIGNED_BYTE, pixels);

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

static bool get_format(SDL_PixelFormatEnum sdl_format, GLint* tex_format, GLenum* img_format)
{
  switch(sdl_format) {
    case SDL_PIXELFORMAT_ABGR8888:
    case SDL_PIXELFORMAT_BGR888:
      *tex_format = GL_RGBA;
      *img_format = GL_RGBA;
      break;
    case SDL_PIXELFORMAT_ARGB8888:
    case SDL_PIXELFORMAT_RGB888:
      *tex_format = GL_RGBA;
      *img_format = GL_BGRA_EXT;
      break;
    case SDL_PIXELFORMAT_RGB24:
      *tex_format = GL_RGB;
      *img_format = GL_RGB;
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

  GLuint texture_id = 0;
  GLint tex_format;
  GLenum img_format;
  if( get_format(img->format->format,&tex_format,&img_format) ) {
    texture_id = create_texture_from_pixels(img->w,img->h,img->pixels,tex_format,img_format,antialias);
  }else{
    SDL_Surface* tmp = SDL_ConvertSurfaceFormat(img, SDL_PIXELFORMAT_ABGR8888, 0);
    if(tmp) {
      tex_format = GL_RGBA;
      img_format = GL_RGBA;
      texture_id = create_texture_from_pixels(tmp->w,tmp->h,tmp->pixels,tex_format,img_format,antialias);
      SDL_FreeSurface(tmp);
    }else{
      LOG("ConvertSurfaceFormat failed. Format:%s Error:%s",
          SDL_GetPixelFormatName(img->format->format), SDL_GetError() );
    }
  }

  // convert to Premulitiplied Alpha Texture
  if(tex_format==GL_RGBA){
    texture->texture_id = bi_texture_convert_to_premultiplied_alpha(texture_id,tex_format,img->w,img->h);
    texture->w = img->w;
    texture->h = img->h;
    texture->_texture_unit = 0;
  }else{
    texture->texture_id = texture_id;
    texture->w = img->w;
    texture->h = img->h;
    texture->_texture_unit = 0;
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
  memset(texture_mapping->_uv,0,sizeof(texture_mapping->_uv));
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
  GLfloat tw = t->texture->w;
  GLfloat th = t->texture->h;
  t->_uv[0] = x / tw;
  t->_uv[1] = (y+h) / th;
  t->_uv[2] = (x+w) / tw;
  t->_uv[3] = y / th;
}
