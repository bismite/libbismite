#ifndef __BISMITE_GL_H__
#define __BISMITE_GL_H__

#if defined(__EMSCRIPTEN__)
#include <GLES3/gl3.h>
#elif defined(__APPLE__)
#include <OpenGL/gl3.h>
#else
#include <GL/gl.h>
#include <GL/glext.h>
#define GL_FUNCTION_LOAD_REQUIRE 1
#endif

//
// Blend Factor
//
typedef struct  {
  GLenum src;
  GLenum dst;
  GLenum alpha_src;
  GLenum alpha_dst;
} BiBlendFactor;

#define BI_BLEND_FACTOR_DEFAULT (BiBlendFactor){GL_ONE,GL_ONE_MINUS_SRC_ALPHA,GL_ONE,GL_ONE_MINUS_SRC_ALPHA}
#define BI_BLEND_FACTOR_ADDTIVE (BiBlendFactor){GL_SRC_ALPHA,GL_ONE,GL_SRC_ALPHA,GL_ONE}

static inline BiBlendFactor bi_blend_factor(GLenum src, GLenum dst, GLenum alpha_src, GLenum alpha_dst ) {
  return (BiBlendFactor){src,dst,alpha_src,alpha_dst};
}

//
// Functions
//
#ifdef GL_FUNCTION_LOAD_REQUIRE
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
extern PFNGLGETPROGRAMIVPROC glGetProgramiv;
extern PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog;
extern PFNGLGETPROGRAMINFOLOGPROC glGetProgramInfoLog;
extern PFNGLATTACHSHADERPROC  glAttachShader;
extern PFNGLDELETESHADERPROC  glDeleteShader;
extern PFNGLCREATEPROGRAMPROC glCreateProgram;
extern PFNGLLINKPROGRAMPROC  glLinkProgram;
extern PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation;
extern PFNGLGETATTRIBLOCATIONPROC glGetAttribLocation;
extern PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray;
extern PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer;
// GL_VERSION_3_0
extern PFNGLBLITFRAMEBUFFERPROC glBlitFramebuffer;
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

#endif // GL_FUNCTION_LOAD_REQUIRE

#endif // __BISMITE_GL_H__
