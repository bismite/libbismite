#include <bi/framebuffer.h>
#include <bi/node.h>
#include <bi/image.h>

BiFramebuffer* bi_framebuffer_init_with_texture_num(BiFramebuffer *fb,int w,int h,int texture_num)
{
  fb->w = w;
  fb->h = h;
  fb->viewport_w = w;
  fb->viewport_h = h;
  fb->texture_num = texture_num;
  glGenFramebuffers(1, &fb->framebuffer_id);
  if(texture_num>0){
    GLuint texs[BI_FRAMEBUFFER_TEXTURE_MAX];
    glGenTextures(texture_num, texs);
    for(int i=0;i<texture_num;i++){
      glBindTexture(GL_TEXTURE_2D, texs[i]);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w,h, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
      glBindTexture(GL_TEXTURE_2D, 0);
      bi_texture_init_with_texture_id(&fb->textures[i],w,h,texs[i]);
    }
  }
  return fb;
}

void bi_framebuffer_clear(BiFramebuffer *fb,uint8_t r,uint8_t g,uint8_t b,uint8_t a)
{
  glBindFramebuffer(GL_FRAMEBUFFER, fb->framebuffer_id);
  if(fb->texture_num>0){
    GLenum list[BI_FRAMEBUFFER_TEXTURE_MAX];
    for(int i=0;i<fb->texture_num;i++){
      list[i] = GL_COLOR_ATTACHMENT0+i;
      glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0+i, GL_TEXTURE_2D, fb->textures[i].texture_id, 0);
    }
    glDrawBuffers(fb->texture_num,list);
  }
  glClearColor(r/255.0, g/255.0, b/255.0, a/255.0);
  glClear(GL_COLOR_BUFFER_BIT);
  // unbind
  if(fb->texture_num>0){
    for(int i=0;i<fb->texture_num;i++){
      glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0+i, GL_TEXTURE_2D, 0, 0);
    }
    glDrawBuffers(0,NULL);
  }
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
