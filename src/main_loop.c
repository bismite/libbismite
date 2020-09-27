#include <bi/main_loop.h>
#include <bi/context.h>
#include <bi/node.h>
#include <bi/render.h>
#include <bi/util.h>
#include <bi/logger.h>

static bool node_event_handle(BiNode* n,BiContext* context,SDL_Event *e)
{
  bool swallow = false;
  switch(e->type){
    case SDL_MOUSEMOTION:
      if(n->_on_move_cursor!=NULL){
        int y = context->h - e->motion.y;
        swallow = n->_on_move_cursor(n,n->_on_move_cursor_context,e->motion.x,y);
      }
      break;
    case SDL_MOUSEBUTTONDOWN:
      if(n->_on_click!=NULL){
        int y = context->h - e->button.y;
        swallow = n->_on_click(n,n->_on_click_context,e->button.x,y,e->button.button,true);
      }
      break;
    case SDL_MOUSEBUTTONUP:
      if(n->_on_click!=NULL){
        int y = context->h - e->button.y;
        swallow = n->_on_click(n,n->_on_click_context,e->button.x,y,e->button.button,false);
      }
      break;
    case SDL_FINGERMOTION:
      if(n->_on_move_finger!=NULL){
        float y = 1.0 - e->tfinger.y;
        swallow = n->_on_move_finger(n,n->_on_move_finger_context, e->tfinger.x, y, e->tfinger.fingerId);
      }
      break;
    case SDL_FINGERDOWN:
      if(n->_on_touch!=NULL){
        float y = 1.0 - e->tfinger.y;
        swallow = n->_on_touch(n,n->_on_touch_context,e->tfinger.x,y,e->tfinger.fingerId,true);
      }
      break;
    case SDL_FINGERUP:
      if(n->_on_touch!=NULL){
        float y = 1.0 - e->tfinger.y;
        swallow = n->_on_touch(n,n->_on_touch_context,e->tfinger.x,y,e->tfinger.fingerId,false);
      }
      break;
    case SDL_KEYDOWN:
      if(n->_on_keyinput!=NULL){
        swallow = n->_on_keyinput(n,n->_on_keyinput_context,e->key.keysym.scancode,e->key.keysym.sym,e->key.keysym.mod,true);
      }
      break;
    case SDL_KEYUP:
      if(n->_on_keyinput!=NULL){
        swallow = n->_on_keyinput(n,n->_on_keyinput_context,e->key.keysym.scancode,e->key.keysym.sym,e->key.keysym.mod,false);
      }
      break;
    case SDL_TEXTINPUT:
      if(n->_on_textinput!=NULL){
        swallow = n->_on_textinput(n,n->_on_textinput_context,e->text.text);
      }
      break;
  }
  return swallow;
}

static void main_loop( void* arg )
{
    int64_t now = bi_get_now();
    BiContext *context = (BiContext*)arg;
    context->frame_start_at = now;

    bi_profile_record(&context->profile,now);

    context->profile.on_update_callbacks_size = context->on_update_callbacks_size;
    context->profile.callback_planned_nodes_size = context->callback_planned_nodes_size;

    //
    // callback and event handling
    //

    for(int i=0;i<context->on_update_callbacks_size;i++){
      context->on_update_callbacks[i].callback(context, context->on_update_callbacks[i].userdata);
    }

    const int PUMP_EVENT_MAX = 32;
    SDL_Event e[PUMP_EVENT_MAX];
    SDL_PumpEvents();
    int event_size = SDL_PeepEvents(e,PUMP_EVENT_MAX,SDL_GETEVENT,SDL_FIRSTEVENT,SDL_LASTEVENT);
    // callback
    for(int i=context->callback_planned_nodes_size-1;i>=0;i--){
      BiNode* n = context->callback_planned_nodes[i];
      if( n == NULL ){
        continue;
      }
      // On Update
      if(n->_on_update.callback != NULL) {
        n->_on_update.callback(context,n);
      }
      // Timer
      bi_run_timers(&n->timers,now);
      // Event Handler
      if( bi_node_has_callback(n) && n->_final_visibility ) {
        for(int i=0;i<event_size;i++) {
          if(e[i].type == 0) continue;
          bool swallow = node_event_handle(n,context,&e[i]);
          if(swallow) {
            e[i].type = 0; // XXX: swallow
          }
        }
      }
    }

    // failsafe
    for(int i=0;i<event_size;i++) {
      if(e[i].type==SDL_QUIT){
          LOG("Program quit after %i ticks", e[i].quit.timestamp);
          context->running = false;
      }
    }

    // reset queue
    context->callback_planned_nodes_size = 0;

    int64_t phase2 = bi_get_now();

    //
    // rendering
    //

    // clear
    glClearColor(
      context->color[0]/255.f,
      context->color[1]/255.f,
      context->color[2]/255.f,
      context->color[3]/255.f
    );
    glClear(GL_COLOR_BUFFER_BIT);

    // reset stats
    context->profile.matrix_updated = 0;
    context->profile.rendering_nodes_queue_size = 0;
    //
    for(int i=0;i<context->layers_size;i++) {
      bi_render_layer(context,context->layers[i]);
    }

    int64_t phase3 = bi_get_now();

    //
    SDL_GL_SwapWindow(context->window);

    //
    context->profile.time_spent_on_callback = phase2 - now;
    context->profile.time_spent_on_rendering = phase3 - phase2;
}

void bi_start_run_loop(BiContext* context)
{
#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop_arg(main_loop, context, context->profile.target_fps, false);
#else
    // fallback
    if( context->profile.target_fps == 0 ) {
      context->profile.target_fps = 60;
    }
    SDL_GL_SetSwapInterval(0);
    while (context->running) {
        double start_at = bi_get_now();
        main_loop(context);
        double end_at = bi_get_now();
        int sleep = 1;
        if(context->profile.target_fps>0) { sleep = (1.0 / context->profile.target_fps) * 1000 - (end_at - start_at); }
        if(sleep<=0) { sleep = 1; }
        SDL_Delay(sleep);
    }
#endif
}
