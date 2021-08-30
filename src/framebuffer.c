#include <bi/layer.h>
#include <bi/node.h>

void bi_framebuffer_init(BiFramebuffer *fb)
{
  glGenFramebuffers(1, &fb->framebuffer_id);
  glBindFramebuffer(GL_FRAMEBUFFER, fb->framebuffer_id);
  glGenTextures(1, &fb->texture_id);
  glBindTexture(GL_TEXTURE_2D, fb->texture_id);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  GLint dims[4] = {0};
  glGetIntegerv(GL_VIEWPORT, dims);
  fb->w = dims[2];
  fb->h = dims[3];
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, fb->w,fb->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fb->texture_id, 0);
  // unbind
  glBindTexture(GL_TEXTURE_2D, 0);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
