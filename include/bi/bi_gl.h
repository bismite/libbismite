#ifndef __BI_CORE_GL_H__
#define __BI_CORE_GL_H__

#if defined(__linux__)
#define GL_GLEXT_PROTOTYPES
#endif

#if defined(__EMSCRIPTEN__)
#include <GLES3/gl3.h>

#elif defined(__APPLE__)
#include <OpenGL/gl3.h>

#else
#include <GL/gl.h>
#include <GL/glext.h>
#endif

#if !defined(__EMSCRIPTEN__) && !defined(__APPLE__)
extern PFNGLACTIVETEXTUREPROC glActiveTexture;
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

#endif // __BI_CORE_GL_H__
