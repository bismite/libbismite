#ifndef __BI_CORE_GL_H__
#define __BI_CORE_GL_H__

#if defined(__EMSCRIPTEN__)
#include <GLES3/gl3.h>

#elif defined(__APPLE__)
#include <OpenGL/gl3.h>

#else // !defined(__EMSCRIPTEN__) && !defined(__APPLE__)
#include <GL/gl.h>
#include <GL/glext.h>

#if defined(__MINGW32__)
// GL_VERSION_1_3
extern PFNGLACTIVETEXTUREPROC glActiveTexture;
#endif // defined(__MINGW32__)

// GL_VERSION_1_4
extern PFNGLBLENDFUNCSEPARATEPROC glBlendFuncSeparate;
// GL_VERSION_1_5
extern PFNGLBINDBUFFERPROC glBindBuffer;
extern PFNGLBUFFERDATAPROC glBufferData;
extern PFNGLBUFFERSUBDATAPROC glBufferSubData;
extern PFNGLGENBUFFERSPROC glGenBuffers;
// GL_VERSION_2_0
extern PFNGLUSEPROGRAMPROC glUseProgram;
extern PFNGLUNIFORM1IVPROC glUniform1iv;
extern PFNGLUNIFORM1IPROC glUniform1i;
extern PFNGLUNIFORM1FPROC glUniform1f;
extern PFNGLUNIFORM2FPROC glUniform2f;
extern PFNGLUNIFORM4FVPROC glUniform4fv;
extern PFNGLUNIFORMMATRIX4FVPROC glUniformMatrix4fv;
extern PFNGLCREATESHADERPROC glCreateShader;
extern PFNGLSHADERSOURCEPROC glShaderSource;
extern PFNGLCOMPILESHADERPROC glCompileShader;
extern PFNGLGETSHADERIVPROC glGetShaderiv;
extern PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog;
extern PFNGLATTACHSHADERPROC  glAttachShader;
extern PFNGLDELETESHADERPROC  glDeleteShader;
extern PFNGLCREATEPROGRAMPROC glCreateProgram;
extern PFNGLLINKPROGRAMPROC  glLinkProgram;
extern PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation;
extern PFNGLGETATTRIBLOCATIONPROC glGetAttribLocation;
extern PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray;
extern PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer;
// GL_VERSION_3_0
extern PFNGLGENVERTEXARRAYSPROC glGenVertexArrays;
extern PFNGLBINDVERTEXARRAYPROC glBindVertexArray;
extern PFNGLBINDFRAMEBUFFERPROC glBindFramebuffer;
extern PFNGLGENFRAMEBUFFERSPROC glGenFramebuffers;
extern PFNGLDELETEFRAMEBUFFERSPROC glDeleteFramebuffers;
extern PFNGLFRAMEBUFFERTEXTURE2DPROC glFramebufferTexture2D;
extern PFNGLVERTEXATTRIBIPOINTERPROC glVertexAttribIPointer;
// GL_VERSION_3_1
extern PFNGLDRAWARRAYSINSTANCEDPROC glDrawArraysInstanced;
// GL_VERSION_3_3
extern PFNGLVERTEXATTRIBDIVISORPROC glVertexAttribDivisor;

#endif // !defined(__EMSCRIPTEN__) && !defined(__APPLE__)

#endif // __BI_CORE_GL_H__
