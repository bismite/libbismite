#ifndef __BI_CORE_CONTEXT_H__
#define __BI_CORE_CONTEXT_H__

#include <stdbool.h>
#include <GL/glew.h>
#include <bi/bi_sdl.h>
#include <bi/node.h>
#include <bi/shader.h>
#include <bi/profile.h>
#include <bi/timer.h>

struct _BiContext;
typedef struct _BiContext BiContext;

struct _BiNode;
typedef struct _BiNode BiNode;

struct _BiLayer;
typedef struct _BiLayer BiLayer;

struct _BiContext {
    bool running;

    // geometry
    int w;
    int h;
    int camera_x;
    int camera_y;

    // background color
    uint8_t color[4];

    // fps
    BiProfile profile;

    // show debug info
    bool debug;

    // Layers
    BiLayer* layers[0xFF];
    int layers_size;

    // rendering queue
    BiNode* rendering_nodes_queue[0xFFFF];
    int rendering_nodes_queue_size;

    // event and update callback
    BiNode* callback_planned_nodes[0xFFFF];
    int callback_planned_nodes_size;

    // timer callback
    BiTimer* timers[0xFFFF];
    int timers_size;

    //
    BiShader shader;

    //
    SDL_Window *window;

    //
    void* userdata;
};

typedef void (*InitializeFunction)(BiContext*);

extern void bi_init_context(BiContext* context,int w, int h, int fps, const char* title, InitializeFunction func );
extern void bi_set_title(BiContext* context,const char* title);

// layer
extern void bi_layer_init(BiLayer* layer);
extern void bi_add_layer(BiContext* context, BiLayer* layer);
extern void bi_remove_layer(BiContext* context, BiLayer* layer);

// timer
extern void bi_timer_init(BiTimer* timer, BiNode* node, timer_callback callback, double interval, int repeat, void* userdata);
extern void bi_add_timer(BiContext* context, BiTimer* timer);
extern void bi_remove_timer(BiContext* context, BiTimer* timer);

#endif
