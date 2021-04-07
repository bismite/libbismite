#ifndef __BI_CORE_GL_H__
#define __BI_CORE_GL_H__

#if defined(__linux__) || defined(__APPLE__)
#define GL_GLEXT_PROTOTYPES
#endif

#ifdef __EMSCRIPTEN__
#include <SDL_opengles2.h>
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
extern PFNGLACTIVETEXTUREPROC glActiveTexture;
extern PFNGLUSEPROGRAMPROC glUseProgram;
extern PFNGLUNIFORM1FPROC glUniform1f;
extern PFNGLUNIFORM2FPROC glUniform2f;
extern PFNGLUNIFORM4FVPROC glUniform4fv;
extern PFNGLBINDBUFFERPROC glBindBuffer;
extern PFNGLBUFFERDATAPROC glBufferData;
extern PFNGLBUFFERSUBDATAPROC glBufferSubData;
extern PFNGLCREATESHADERPROC glCreateShader;
extern PFNGLSHADERSOURCEPROC glShaderSource;
extern PFNGLCOMPILESHADERPROC glCompileShader;
extern PFNGLATTACHSHADERPROC  glAttachShader;
extern PFNGLBINDFRAMEBUFFERPROC glBindFramebuffer;
extern PFNGLCREATEPROGRAMPROC glCreateProgram;
extern PFNGLLINKPROGRAMPROC  glLinkProgram;
extern PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation;
extern PFNGLGETATTRIBLOCATIONPROC glGetAttribLocation;
extern PFNGLUNIFORM1IVPROC glUniform1iv;
extern PFNGLUNIFORMMATRIX4FVPROC glUniformMatrix4fv;
extern PFNGLGENBUFFERSPROC glGenBuffers;
extern PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray;
extern PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer;
extern PFNGLGENFRAMEBUFFERSPROC glGenFramebuffers;
extern PFNGLFRAMEBUFFERTEXTURE2DPROC glFramebufferTexture2D;
#endif
#endif

#endif // __BI_CORE_GL_H__
