#ifndef __BI_CORE_GL_H__
#define __BI_CORE_GL_H__

#ifdef __EMSCRIPTEN__
#  include <SDL_opengles2.h>
#else
#  include <SDL_opengl.h>
#  include <SDL_opengl_glext.h>
#endif

extern void (*glGenVertexArrays_wrapper)(GLsizei, GLuint*);
extern void (*glBindVertexArray_wrapper)(GLuint);
extern void (*glDrawArraysInstanced_wrapper)(GLenum, int, GLsizei, GLsizei);
extern void (*glVertexAttribDivisor_wrapper)(GLuint,GLuint);

#endif
