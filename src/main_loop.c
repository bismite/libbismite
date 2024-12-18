#include <bi/main_loop.h>
#include <bi/timer.h>
#include <bi/context.h>
#include <bi/node.h>
#include <bi/render.h>
#include <bi/util.h>
#include <bi/node_base.h>
#include <bi/logger.h>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

static bool node_event_handle(BiNode* n,BiContext* context,SDL_Event *e)
{
  bool swallow = false;
  switch(e->type){
    case SDL_MOUSEMOTION:
      if(n->on_move_cursor!=NULL){
        int y = context->h - e->motion.y;
        swallow = n->on_move_cursor(context,n,e->motion.x,y);
      }
      break;
    case SDL_MOUSEBUTTONDOWN:
      if(n->on_click!=NULL){
        int y = context->h - e->button.y;
        swallow = n->on_click(context,n,e->button.x,y,e->button.button,true);
      }
      break;
    case SDL_MOUSEBUTTONUP:
      if(n->on_click!=NULL){
        int y = context->h - e->button.y;
        swallow = n->on_click(context,n,e->button.x,y,e->button.button,false);
      }
      break;
    case SDL_FINGERMOTION:
      if(n->on_move_finger!=NULL){
        float y = 1.0 - e->tfinger.y;
        swallow = n->on_move_finger(context,n, e->tfinger.x, y, e->tfinger.fingerId);
      }
      break;
    case SDL_FINGERDOWN:
      if(n->on_touch!=NULL){
        float y = 1.0 - e->tfinger.y;
        swallow = n->on_touch(context,n,e->tfinger.x,y,e->tfinger.fingerId,true);
      }
      break;
    case SDL_FINGERUP:
      if(n->on_touch!=NULL){
        float y = 1.0 - e->tfinger.y;
        swallow = n->on_touch(context,n,e->tfinger.x,y,e->tfinger.fingerId,false);
      }
      break;
    case SDL_KEYDOWN:
      if(n->on_keyinput!=NULL){
        swallow = n->on_keyinput(context,n,e->key.keysym.scancode,e->key.keysym.sym,e->key.keysym.mod,true);
      }
      break;
    case SDL_KEYUP:
      if(n->on_keyinput!=NULL){
        swallow = n->on_keyinput(context,n,e->key.keysym.scancode,e->key.keysym.sym,e->key.keysym.mod,false);
      }
      break;
    case SDL_TEXTINPUT:
      if(n->on_textinput!=NULL){
        swallow = n->on_textinput(context,n,e->text.text);
      }
      break;
  }
  return swallow;
}

static inline void timers_compact(BiTimers* timers)
{
  int new_size = 0;
  for(int i=0;i<timers->size;i++){
    BiTimer *t = timers->timers[i];
    if(t!=NULL && t->count != 0){
      timers->timers[new_size] = timers->timers[i];
      new_size++;
    }
  }
  if( timers->size != new_size ){
    timers->size = new_size;
    timers->timers = realloc( timers->timers, sizeof(BiTimer*) * timers->size );
  }
}

static inline void __run_timers__(BiContext* context,double delta_time)
{
  for(int i=context->timer_queue.size-1;i>=0;i--){
    BiNodeBase *node = context->timer_queue.objects[i];
    if( node == NULL ) continue;
    BiTimers* timers = &node->timers;
    double dt = delta_time * timers->_final_time_scale;
    for(int j=0;j<timers->size;j++){
      BiTimer* t = timers->timers[j];
      // skip
      if(t==NULL) continue; // timer removed
      if(t->state == BI_TIMER_STATE_PAUSED) continue;
      if(t->count == 0) continue;
      // check schedule
      t->wait -= dt;
      if(t->wait < 0) {
        t->count -= 1;
        t->wait = t->interval;
        t->callback(context,t,dt);
      }
      // node removed
      if( context->timer_queue.objects[i] == NULL )
        break;
    }
    // remove and resize
    timers_compact(timers);
  }
}

static void main_loop( void* arg )
{
  BiContext *context = (BiContext*)arg;
  context->frame_start_at = bi_get_now();
  double delta_time = context->last_update==0 ? 0 : (context->frame_start_at - context->last_update);
  if(delta_time > context->max_delta) delta_time = context->max_delta;
  context->last_update = context->frame_start_at;

  bi_profile_record(&context->profile,context->frame_start_at);

  context->profile.callback_planned_nodes_size = context->interaction_queue.size;

  //
  // callback and event handling
  //
  const int PUMP_EVENT_MAX = 32;
  SDL_Event e[PUMP_EVENT_MAX];
  SDL_PumpEvents();
  int event_size = SDL_PeepEvents(e,PUMP_EVENT_MAX,SDL_GETEVENT,SDL_FIRSTEVENT,SDL_LASTEVENT);
  // Window Event
  for(int i=0;i<event_size;i++) {
    if(e[i].type == SDL_WINDOWEVENT ){
      // Update Default Framebuffer Viewport
      if(e[i].window.event == SDL_WINDOWEVENT_RESIZED){
        SDL_GetWindowSizeInPixels(context->_window,
          &context->default_framebuffer.viewport_w,
          &context->default_framebuffer.viewport_h );
      }
      window_event_callback cb = context->window_event_callback[e[i].window.event];
      if(cb) cb(context);
    }
  }
  // Timer
  __run_timers__(context,delta_time);
  // callback
  for(int i=0;i<event_size;i++) {
    for(int j=context->interaction_queue.size-1;j>=0;j--){
      BiNodeBase *n = context->interaction_queue.objects[j];
      if( n == NULL ) continue;
      // Event Handler
      BiNode* node = (BiNode*)n;
      if( node->_final_visibility ) {
        bool swallow = node_event_handle(node,context,&e[i]);
        if(swallow) {
          break;
        }
      }
    }
  }

  // quit
  for(int i=0;i<event_size;i++) {
    if(e[i].type==SDL_QUIT){
        context->running = false;
    }
  }

  // reset queue
  array_clear(&context->interaction_queue);
  array_clear(&context->timer_queue);

  int64_t phase2 = bi_get_now();

  //
  // rendering
  //
  bi_framebuffer_clear(&context->default_framebuffer,0,0,0,0);
  context->profile.matrix_updated = 0;
  context->profile.rendering_nodes_queue_size = 0;
  int64_t time = (context->program_start_at - context->frame_start_at);
  int real_w,real_h;
  SDL_GetWindowSizeInPixels(context->_window,&real_w,&real_h);
  BiRenderingContext rendering_context;
  bi_rendering_context_init(&rendering_context, time,
                            &context->default_shader,
                            &context->interaction_queue,
                            &context->timer_queue );
  bi_render_node(rendering_context, &context->default_framebuffer_node );
  SDL_GL_SwapWindow(context->_window);

  //
  int64_t phase3 = bi_get_now();

  //
  context->profile.time_spent_on_callback = phase2 - context->frame_start_at;
  context->profile.time_spent_on_rendering = phase3 - phase2;

  //
  if(context->on_mainloop_end) context->on_mainloop_end(context);
}

void bi_stop_run_loop()
{
#ifdef __EMSCRIPTEN__
  emscripten_cancel_main_loop();
#else
  // nop
#endif
}

void bi_start_run_loop(BiContext* context)
{
  context->running = true;
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
