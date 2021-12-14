#ifndef __BI_CORE_GL_H__
#define __BI_CORE_GL_H__

#if defined(__linux__)
#define GL_GLEXT_PROTOTYPES
#endif

#if defined(__EMSCRIPTEN__)
#include <SDL_opengles2.h>
#elif defined(__APPLE__)
#include <SDL_opengl.h>
#include <SDL_opengl_glext.h>
#else
#include <GL/gl.h>
#include <GL/glext.h>
#endif

#ifndef GL_GLEXT_PROTOTYPES
extern void (*glGenVertexArrays)(GLsizei, GLuint*);
extern void (*glBindVertexArray)(GLuint);
extern void (*glDrawArraysInstanced)(GLenum, int, GLsizei, GLsizei);
extern void (*glVertexAttribDivisor)(GLuint,GLuint);

#ifndef __EMSCRIPTEN__
#ifndef __APPLE__
extern PFNGLACTIVETEXTUREPROC glActiveTexture;
#endif
extern PFNGLUSEPROGRAMPROC glUseProgram;
extern PFNGLUNIFORM1IVPROC glUniform1iv;
extern PFNGLUNIFORM1IPROC glUniform1i;
extern PFNGLUNIFORM1FPROC glUniform1f;
extern PFNGLUNIFORM2FPROC glUniform2f;
extern PFNGLUNIFORM4FVPROC glUniform4fv;
extern PFNGLUNIFORMMATRIX4FVPROC glUniformMatrix4fv;
extern PFNGLBINDBUFFERPROC glBindBuffer;
extern PFNGLBUFFERDATAPROC glBufferData;
extern PFNGLBUFFERSUBDATAPROC glBufferSubData;
extern PFNGLCREATESHADERPROC glCreateShader;
extern PFNGLSHADERSOURCEPROC glShaderSource;
extern PFNGLCOMPILESHADERPROC glCompileShader;
extern PFNGLGETSHADERIVPROC glGetShaderiv;
extern PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog;
extern PFNGLATTACHSHADERPROC  glAttachShader;
extern PFNGLDELETESHADERPROC  glDeleteShader;
extern PFNGLBINDFRAMEBUFFERPROC glBindFramebuffer;
extern PFNGLCREATEPROGRAMPROC glCreateProgram;
extern PFNGLLINKPROGRAMPROC  glLinkProgram;
extern PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation;
extern PFNGLGETATTRIBLOCATIONPROC glGetAttribLocation;
extern PFNGLGENBUFFERSPROC glGenBuffers;
extern PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray;
extern PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer;
extern PFNGLGENFRAMEBUFFERSPROC glGenFramebuffers;
extern PFNGLDELETEFRAMEBUFFERSPROC glDeleteFramebuffers;
extern PFNGLFRAMEBUFFERTEXTURE2DPROC glFramebufferTexture2D;
extern PFNGLBLENDFUNCSEPARATEPROC glBlendFuncSeparate;
#endif
#endif

#endif // __BI_CORE_GL_H__
