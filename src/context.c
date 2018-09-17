#include <bi/context.h>
#include <bi/util.h>
#include <bi/shader.h>
#include <bi/profile.h>
#include <bi/timer.h>
#include <bi/gl_ext.h>
#include <bi/layer.h>
#include <bi/bi_sdl.h>
#include <bi/logger.h>
#include <stdlib.h>

void bi_init_context(BiContext* context,int w,int h, int fps, const char* title, InitializeFunction func)
{
    context->rendering_nodes_queue_size = 0;
    context->callback_planned_nodes_size = 0;
    context->timers_size = 0;
    context->layers_size = 0;

    //
    double now = bi_get_now();
    bi_profile_init(&context->profile,fps,now);

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
    LOG("EMSCRIPTEN\n");
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#elif __APPLE__
    LOG("APPLE\n");
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
#elif __MINGW32__
    LOG("MINGW32\n");
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
#else
    LOG("unknown platform\n");
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
#endif

    context->window = SDL_CreateWindow(title, 0, 0, w, h, SDL_WINDOW_OPENGL);
    // SDL_GLContext sdl_gl_context = SDL_GL_CreateContext(context->window);
    SDL_GL_CreateContext(context->window);

    if( glewInit() == GLEW_OK ){
      LOG("glewInit ok\n");
    }else{
      LOG("glewInit faild\n");
    }

    checkSupports();

    bi_init_shader(&context->shader,context->w,context->h);

    // initialize
    if(func!=NULL){
      func(context);
    }
}

void bi_set_title(BiContext* context, const char* title)
{
  SDL_SetWindowTitle( context->window, title );
}

//
// Layer
//

void bi_layer_init(BiLayer* layer)
{
  layer->root = NULL;
  layer->camera_x = 0;
  layer->camera_y = 0;
  layer->projection_centering = false;
  layer->blend_src = GL_SRC_ALPHA;
  layer->blend_dst = GL_ONE_MINUS_SRC_ALPHA;
}

void bi_add_layer(BiContext* context, BiLayer* layer)
{
  context->layers[context->layers_size] = layer;
  context->layers_size += 1;
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

//
// Timer
//

void bi_timer_init(BiTimer* timer, BiNode* node, timer_callback callback, double interval, int repeat, void* userdata)
{
    timer->repeat = repeat;
    timer->interval = interval;
    timer->will_fire_at = 0; // XXX: set at next frame start
    timer->last_fire_at = 0;
    timer->node = node;
    timer->callback = callback;
    timer->userdata = userdata;
}

void bi_add_timer(BiContext* context, BiTimer* timer)
{
    // TODO: duplicate check
    context->timers[context->timers_size] = timer;
    context->timers_size += 1;
}

void bi_remove_timer(BiContext* context, BiTimer* timer)
{
  int index = -1;
  for(int i=0;i<context->timers_size;i++){
    if( context->timers[i] == timer ) {
      index = i;
      break;
    }
  }
  for(int i=index+1;i<context->timers_size;i++) {
    context->timers[i-1] = context->timers[i];
  }
  context->timers_size -= 1;
}
