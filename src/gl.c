#include <bi/bi_gl.h>
#include <bi/context.h>

#if !defined(__EMSCRIPTEN__) && !defined(__APPLE__)

#if defined(__MINGW32__)
// GL_VERSION_1_3
PFNGLACTIVETEXTUREPROC glActiveTexture;
#endif // defined(__MINGW32__)

// GL_VERSION_1_4
PFNGLBLENDFUNCSEPARATEPROC glBlendFuncSeparate;
// GL_VERSION_1_5
PFNGLBINDBUFFERPROC glBindBuffer;
PFNGLBUFFERDATAPROC glBufferData;
PFNGLBUFFERSUBDATAPROC glBufferSubData;
PFNGLGENBUFFERSPROC glGenBuffers;
// GL_VERSION_2_0
PFNGLUSEPROGRAMPROC glUseProgram;
PFNGLUNIFORM1IVPROC glUniform1iv;
PFNGLUNIFORM1IPROC glUniform1i;
PFNGLUNIFORM1FPROC glUniform1f;
PFNGLUNIFORM2FPROC glUniform2f;
PFNGLUNIFORM4FVPROC glUniform4fv;
PFNGLUNIFORMMATRIX4FVPROC glUniformMatrix4fv;
PFNGLCREATESHADERPROC glCreateShader;
PFNGLSHADERSOURCEPROC glShaderSource;
PFNGLCOMPILESHADERPROC glCompileShader;
PFNGLGETSHADERIVPROC glGetShaderiv;
PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog;
PFNGLATTACHSHADERPROC  glAttachShader;
PFNGLDELETESHADERPROC  glDeleteShader;
PFNGLCREATEPROGRAMPROC glCreateProgram;
PFNGLLINKPROGRAMPROC  glLinkProgram;
PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation;
PFNGLGETATTRIBLOCATIONPROC glGetAttribLocation;
PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray;
PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer;
// GL_VERSION_3_0
PFNGLBLITFRAMEBUFFERPROC glBlitFramebuffer;
PFNGLGENVERTEXARRAYSPROC glGenVertexArrays;
PFNGLBINDVERTEXARRAYPROC glBindVertexArray;
PFNGLBINDFRAMEBUFFERPROC glBindFramebuffer;
PFNGLGENFRAMEBUFFERSPROC glGenFramebuffers;
PFNGLDELETEFRAMEBUFFERSPROC glDeleteFramebuffers;
PFNGLFRAMEBUFFERTEXTURE2DPROC glFramebufferTexture2D;
PFNGLVERTEXATTRIBIPOINTERPROC glVertexAttribIPointer;
// GL_VERSION_3_1
PFNGLDRAWARRAYSINSTANCEDPROC glDrawArraysInstanced;
// GL_VERSION_3_3
PFNGLVERTEXATTRIBDIVISORPROC glVertexAttribDivisor;
#endif // !defined(__EMSCRIPTEN__) && !defined(__APPLE__)

#define GLP(name) do{ name = SDL_GL_GetProcAddress( #name ); }while(0)

void enable_gl_extensions()
{
#if !defined(__EMSCRIPTEN__) && !defined(__APPLE__)
#if defined(__MINGW32__)
  // GL_VERSION_1_3
  GLP(glActiveTexture);
#endif
  // GL_VERSION_1_4
  GLP(glBlendFuncSeparate);
  // GL_VERSION_1_5
  GLP(glBindBuffer);
  GLP(glBufferData);
  GLP(glBufferSubData);
  GLP(glGenBuffers);
  // GL_VERSION_2_0
  GLP(glUseProgram);
  GLP(glUniform1i);
  GLP(glUniform1iv);
  GLP(glUniform1f);
  GLP(glUniform2f);
  GLP(glUniform4fv);
  GLP(glUniformMatrix4fv);
  GLP(glCreateShader);
  GLP(glShaderSource);
  GLP(glCompileShader);
  GLP(glGetShaderiv);
  GLP(glGetShaderInfoLog);
  GLP(glAttachShader);
  GLP(glDeleteShader);
  GLP(glCreateProgram);
  GLP(glLinkProgram);
  GLP(glGetUniformLocation);
  GLP(glGetAttribLocation);
  GLP(glEnableVertexAttribArray);
  GLP(glVertexAttribPointer);
  // GL_VERSION_3_0
  GLP(glBlitFramebuffer);
  GLP(glGenVertexArrays);
  GLP(glBindVertexArray);
  GLP(glBindFramebuffer);
  GLP(glGenFramebuffers);
  GLP(glDeleteFramebuffers);
  GLP(glFramebufferTexture2D);
  GLP(glVertexAttribIPointer);
  // GL_VERSION_3_1
  GLP(glDrawArraysInstanced);
  // GL_VERSION_3_3
  GLP(glVertexAttribDivisor);
#endif
}
