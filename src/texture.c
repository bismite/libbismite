#include <bi/texture.h>
#include <bi/bi_gl.h>
#include <bi/bi_sdl.h>
#include <bi/logger.h>
#include <bi/framebuffer.h>
#include <bi/image.h>

extern GLuint bi_texture_convert_to_premultiplied_alpha(GLuint texture_id,GLint tex_format,int w,int h);

//
// Texture
//

static GLuint create_texture_from_pixels(int w,int h,void*pixels,GLint tex_format,GLenum img_format)
{
  GLuint texture_id;
  glGenTextures(1, &texture_id);
  // create texture
  glBindTexture(GL_TEXTURE_2D, texture_id);
  glTexImage2D(GL_TEXTURE_2D, 0, tex_format, w, h, 0, img_format, GL_UNSIGNED_BYTE, pixels);
  // no anti-alias
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
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
    case SDL_PIXELFORMAT_RGB24:
      *tex_format = GL_RGB;
      *img_format = GL_RGB;
      break;
    default:
      return false;
  }
  return true;
}

static BiTexture* load_texture_from_image(BiTexture* texture, SDL_RWops* rwops, bool straight_alpha)
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
    texture_id = create_texture_from_pixels(img->w,img->h,img->pixels,tex_format,img_format);
  }else{
    LOG("Convert image format %s\n", SDL_GetPixelFormatName(img->format->format) );
    SDL_Surface* tmp = SDL_ConvertSurfaceFormat(img, SDL_PIXELFORMAT_ABGR8888, 0);
    if(tmp) {
      tex_format = GL_RGBA;
      img_format = GL_RGBA;
      texture_id = create_texture_from_pixels(tmp->w,tmp->h,tmp->pixels,tex_format,img_format);
      SDL_FreeSurface(tmp);
    }else{
      LOG("Convert image format failed. Format:%s Error:%s\n",
          SDL_GetPixelFormatName(img->format->format), SDL_GetError() );
    }
  }

  // convert to Premulitiplied Alpha Texture
  if(straight_alpha==false && tex_format==GL_RGBA){
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

BiTexture* bi_texture_init_with_file(BiTexture* texture, void* buffer, int size, bool straight_alpha)
{
  SDL_RWops *rw = SDL_RWFromMem(buffer,size);
  if(rw==NULL) {
    LOG("bi_texture_init_with_file : SDL_RWFromMem failed\n");
    return NULL;
  }
  return load_texture_from_image( texture, SDL_RWFromMem(buffer,size), straight_alpha );
}

BiTexture* bi_texture_init_with_filename(BiTexture* texture, const char* filename, bool straight_alpha)
{
  SDL_RWops *rw = SDL_RWFromFile(filename,"rb");
  if(rw==NULL) {
    LOG("bi_texture_init_with_filename : SDL_RWFromFile(%s) failed.\n",filename);
    return NULL;
  }
  return load_texture_from_image( texture, rw, straight_alpha );
}

BiTexture* bi_texture_init_with_texture_id(BiTexture* texture, int w, int h, GLuint texture_id)
{
  texture->texture_id = texture_id;
  texture->w = w;
  texture->h = h;
  texture->_texture_unit = 0;
  return texture;
}

void bi_texture_set_anti_alias(BiTexture* texture,bool anti_alias)
{
  glBindTexture(GL_TEXTURE_2D, texture->texture_id);
  if(anti_alias) {
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  }else{
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  }
  glBindTexture(GL_TEXTURE_2D, 0);
}

void bi_texture_delete(BiTexture* texture)
{
  glDeleteTextures(1,&texture->texture_id);
  texture->texture_id = 0;
  texture->w = 0;
  texture->h = 0;
  texture->_texture_unit = 0;
}

void bi_texture_save_png_image(BiTexture* texture,const char *filename,bool flip_vertical)
{
  const int w = texture->w;
  const int h = texture->h;
  uint8_t* pixels = malloc(w*h*4);
  GLuint fb;
  // !!!: glGetTexImage unavailable in WebGL
  glGenFramebuffers(1, &fb);
  glBindFramebuffer(GL_FRAMEBUFFER, fb);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture->texture_id, 0);
  glReadPixels(0,0,w,h,GL_RGBA,GL_UNSIGNED_BYTE,pixels);
  glBindFramebuffer(GL_FRAMEBUFFER,0);
  glDeleteFramebuffers(1,&fb);
  if(flip_vertical) bi_image_rgba32_flip_vertical(w,h,pixels);
  bi_image_rgba32_save(w,h,pixels,filename);
  free(pixels);
}
