#include <bi/context.h>
#include <bi/util.h>
#include <bi/timer.h>
#include <bi/layer.h>
#include <bi/logger.h>
#include <stdlib.h>
#include <bi/default_shader.h>

#ifndef GL_GLEXT_PROTOTYPES
void (*glGenVertexArrays)(GLsizei, GLuint*);
void (*glBindVertexArray)(GLuint);
void (*glDrawArraysInstanced)(GLenum, int, GLsizei, GLsizei);
void (*glVertexAttribDivisor)(GLuint,GLuint);

#ifndef __EMSCRIPTEN__
PFNGLACTIVETEXTUREPROC glActiveTexture;
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
#define GLP(name) do{ name = SDL_GL_GetProcAddress( #name ); }while(0)
#endif
#endif

static void enable_gl_extensions(BiContext* context)
{
#if ! defined(GL_GLEXT_PROTOTYPES) && ! defined(__EMSCRIPTEN__)
    GLP(glActiveTexture);
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
    context->program_start_at = bi_get_now();

    context->rendering_nodes_queue_size = 0;
    context->callback_planned_nodes_size = 0;
    context->layers_size = 0;

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

    bi_shader_init(&context->default_shader,context->w,context->h, DEFAULT_VERTEX_SHADER, DEFAULT_FRAGMENT_SHADER);

    //
    // post processing
    //
    // frame buffer
    glGenFramebuffers(1, &context->post_processing.framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, context->post_processing.framebuffer);
    glGenTextures(1, &context->post_processing.texture);
    glBindTexture(GL_TEXTURE_2D, context->post_processing.texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    int scaled_w,scaled_h;
    SDL_GL_GetDrawableSize(context->window, &scaled_w, &scaled_h); // !!!: retina display scaled x2!
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, scaled_w, scaled_h, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, context->post_processing.texture, 0);
    // unbind
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    // shader
    context->post_processing.shader = NULL;
}

void bi_set_title(BiContext* context, const char* title)
{
  SDL_SetWindowTitle( context->window, title );
}

//
// Layer
//

static int layer_order_compare(const void *_a, const void *_b )
{
  const BiLayer *a = *(BiLayer**)_a;
  const BiLayer *b = *(BiLayer**)_b;
  return a->z_order == b->z_order ? a->index - b->index : a->z_order - b->z_order;
}

void bi_update_layer_order(BiContext* context)
{
  for(int i=0;i<context->layers_size;i++) { context->layers[i]->index = i; }
  qsort(context->layers,context->layers_size,sizeof(BiLayer*),layer_order_compare);
  for(int i=0;i<context->layers_size;i++) { context->layers[i]->index = i; }
}

void bi_add_layer(BiContext* context, BiLayer* layer)
{
  context->layers[context->layers_size] = layer;
  context->layers_size += 1;
  bi_update_layer_order(context);
}

void bi_remove_layer(BiContext* context, BiLayer* layer)
{
  int index = -1;
  for(int i=0;i<context->layers_size;i++){
    if( context->layers[i] == layer ) {
      index = i;
      break;
    }
  }
  for(int i=index+1;i<context->layers_size;i++) {
    context->layers[i-1] = context->layers[i];
  }
  context->layers_size -= 1;
}
