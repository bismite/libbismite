#include <bi/layer.h>
#include <bi/node.h>

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
