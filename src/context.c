#include <bi/context.h>
#include <bi/util.h>
#include <bi/timer.h>
#include <bi/layer.h>
#include <bi/logger.h>
#include <stdlib.h>
#include "default_shader.h"

#ifndef GL_GLEXT_PROTOTYPES
void (*glGenVertexArrays)(GLsizei, GLuint*);
void (*glBindVertexArray)(GLuint);
void (*glDrawArraysInstanced)(GLenum, int, GLsizei, GLsizei);
void (*glVertexAttribDivisor)(GLuint,GLuint);

#ifndef __EMSCRIPTEN__
#ifndef __APPLE__
PFNGLACTIVETEXTUREPROC glActiveTexture;
#endif
PFNGLUSEPROGRAMPROC glUseProgram;
PFNGLUNIFORM1FPROC glUniform1f;
PFNGLUNIFORM2FPROC glUniform2f;
PFNGLUNIFORM4FVPROC glUniform4fv;
PFNGLBINDBUFFERPROC glBindBuffer;
PFNGLBUFFERDATAPROC glBufferData;
PFNGLBUFFERSUBDATAPROC glBufferSubData;
PFNGLCREATESHADERPROC glCreateShader;
PFNGLSHADERSOURCEPROC glShaderSource;
PFNGLCOMPILESHADERPROC glCompileShader;
PFNGLATTACHSHADERPROC  glAttachShader;
PFNGLDELETESHADERPROC  glDeleteShader;
PFNGLBINDFRAMEBUFFERPROC glBindFramebuffer;
PFNGLCREATEPROGRAMPROC glCreateProgram;
PFNGLLINKPROGRAMPROC  glLinkProgram;
PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation;
PFNGLGETATTRIBLOCATIONPROC glGetAttribLocation;
PFNGLUNIFORM1IVPROC glUniform1iv;
PFNGLUNIFORMMATRIX4FVPROC glUniformMatrix4fv;
PFNGLGENBUFFERSPROC glGenBuffers;
PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray;
PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer;
PFNGLGENFRAMEBUFFERSPROC glGenFramebuffers;
PFNGLFRAMEBUFFERTEXTURE2DPROC glFramebufferTexture2D;
PFNGLBLENDFUNCSEPARATEPROC glBlendFuncSeparate;
#define GLP(name) do{ name = SDL_GL_GetProcAddress( #name ); }while(0)
#endif
#endif

static void enable_gl_extensions(BiContext* context)
{
#if ! defined(GL_GLEXT_PROTOTYPES) && ! defined(__EMSCRIPTEN__)
#ifndef __APPLE__
    GLP(glActiveTexture);
#endif
    GLP(glUseProgram);
    GLP(glUniform1f);
    GLP(glUniform2f);
    GLP(glUniform4fv);
    GLP(glBindBuffer);
    GLP(glBufferData);
    GLP(glBufferSubData);
    GLP(glCreateShader);
    GLP(glShaderSource);
    GLP(glCompileShader);
    GLP(glAttachShader);
    GLP(glDeleteShader);
    GLP(glBindFramebuffer);
    GLP(glCreateProgram);
    GLP(glLinkProgram);
    GLP(glGetUniformLocation);
    GLP(glGetAttribLocation);
    GLP(glUniform1iv);
    GLP(glUniformMatrix4fv);
    GLP(glGenBuffers);
    GLP(glEnableVertexAttribArray);
    GLP(glVertexAttribPointer);
    GLP(glGenFramebuffers);
    GLP(glFramebufferTexture2D);
    GLP(glBlendFuncSeparate);
#endif

#ifndef GL_GLEXT_PROTOTYPES
#ifdef __EMSCRIPTEN__
    // https://www.khronos.org/registry/OpenGL/extensions/OES/OES_vertex_array_object.txt
    if(SDL_GL_ExtensionSupported("GL_OES_vertex_array_object")) {
      glGenVertexArrays = SDL_GL_GetProcAddress("glGenVertexArraysOES");
      glBindVertexArray = SDL_GL_GetProcAddress("glBindVertexArrayOES");
    }

    // https://www.khronos.org/registry/OpenGL/extensions/EXT/EXT_instanced_arrays.txt
    if(SDL_GL_ExtensionSupported("GL_EXT_instanced_arrays")) {
      glDrawArraysInstanced = SDL_GL_GetProcAddress("glDrawArraysInstancedEXT");
      glVertexAttribDivisor = SDL_GL_GetProcAddress("glVertexAttribDivisorEXT");
    }

    // https://www.khronos.org/registry/OpenGL/extensions/ANGLE/ANGLE_instanced_arrays.txt
    if(SDL_GL_ExtensionSupported("GL_ANGLE_instanced_arrays")) {
      glDrawArraysInstanced = SDL_GL_GetProcAddress("glDrawArraysInstancedANGLE");
      glVertexAttribDivisor = SDL_GL_GetProcAddress("glVertexAttribDivisorANGLE");
    }
#else
    // https://www.khronos.org/registry/OpenGL/extensions/APPLE/APPLE_vertex_array_object.txt
    if( SDL_GL_ExtensionSupported("GL_APPLE_vertex_array_object") ) {
        glGenVertexArrays = SDL_GL_GetProcAddress("glGenVertexArraysAPPLE");
        glBindVertexArray = SDL_GL_GetProcAddress("glBindVertexArrayAPPLE");
    // https://www.khronos.org/registry/OpenGL/extensions/ARB/ARB_vertex_array_object.txt
    } else if( SDL_GL_ExtensionSupported("GL_ARB_vertex_array_object")) {
        glGenVertexArrays = SDL_GL_GetProcAddress("glGenVertexArrays");
        glBindVertexArray = SDL_GL_GetProcAddress("glBindVertexArray");
    }

    // https://www.khronos.org/registry/OpenGL/extensions/ARB/ARB_instanced_arrays.txt
    if( SDL_GL_ExtensionSupported("GL_ARB_instanced_arrays")) {
      glVertexAttribDivisor = SDL_GL_GetProcAddress("glVertexAttribDivisorARB");
    }

    // https://www.khronos.org/registry/OpenGL/extensions/ARB/ARB_draw_instanced.txt
    if( SDL_GL_ExtensionSupported("GL_ARB_draw_instanced") ) {
      glDrawArraysInstanced = SDL_GL_GetProcAddress("glDrawArraysInstancedARB");
    // https://www.khronos.org/registry/OpenGL/extensions/EXT/EXT_draw_instanced.txt
    } else if(SDL_GL_ExtensionSupported("GL_EXT_draw_instanced")) {
      glDrawArraysInstanced = SDL_GL_GetProcAddress("glDrawArraysInstancedEXT");
    }
#endif
#endif
}

void bi_init_context(BiContext* context,int w,int h,int fps, bool highdpi, const char* title)
{
    if( SDL_Init(SDL_INIT_VIDEO) != 0 ){
        LOG("SDL_Init fail.");
        return;
    }

    SDL_SetHint(SDL_HINT_RENDER_DRIVER,"opengl");
#ifdef __EMSCRIPTEN__
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#else
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
#endif

    Uint32 flag = SDL_WINDOW_OPENGL;
    if(highdpi == true) { flag = flag | SDL_WINDOW_ALLOW_HIGHDPI; }
    context->window = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, w, h, flag);

    SDL_GLContext *glcontext = SDL_GL_CreateContext(context->window);
    if(glcontext==NULL){
      printf("SDL_GL_CreateContext failed: %s\n",SDL_GetError());
      exit(1);
    }

    enable_gl_extensions(context);
    glEnable(GL_BLEND);

    context->program_start_at = bi_get_now();

    context->rendering_nodes_queue_size = 0;
    context->callback_planned_nodes_size = 0;
    bi_layer_group_init(&context->layers);

    // timers
    context->timers.size = 0;
    context->timers.timers = NULL;

    //
    bi_profile_init(&context->profile,fps,bi_get_now());

    context->debug = false;

    context->running = true;

    context->w = w;
    context->h = h;
    context->camera_x = 0;
    context->camera_y = 0;

    context->color[0] = 0;
    context->color[1] = 0;
    context->color[2] = 0;
    context->color[3] = 0;

    // default shader
    bi_shader_init(&context->default_shader,context->w,context->h, DEFAULT_VERTEX_SHADER, DEFAULT_FRAGMENT_SHADER);

    // default texture
    uint8_t pixels[4] = {0,0,0,0};
    glGenTextures(1, &context->default_texture);
    glBindTexture(GL_TEXTURE_2D, context->default_texture);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void bi_set_title(BiContext* context, const char* title)
{
  SDL_SetWindowTitle( context->window, title );
}

//
// Layer
//
void bi_add_layer(BiContext* context, BiLayer* layer) { bi_layer_group_add_layer(&context->layers,layer); }
void bi_update_layer_order(BiContext* context) { bi_layer_group_update_order(&context->layers); }
void bi_remove_layer(BiContext* context, BiLayer* layer) { bi_layer_group_remove_layer(&context->layers,layer); }
