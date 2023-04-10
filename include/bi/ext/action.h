#ifndef __BI_EXT_ACTION_H__
#define __BI_EXT_ACTION_H__

#include <bi/node.h>
#include <bi/timer.h>

typedef struct _BiAction BiAction;

typedef enum {
  BI_ACTION_STATE_READY,
  BI_ACTION_STATE_RUNNING,
  BI_ACTION_STATE_FINISHED
} BiActionState;

typedef void (*bi_action_start_function)(BiAction*); // action
typedef void (*bi_action_update_function)(BiAction*,double); // action,rate
typedef void (*bi_action_on_finish_callback)(BiAction*,void*); // action,context

typedef struct _BiAction {
  bi_action_start_function start;
  bi_action_update_function update;
  bi_action_on_finish_callback on_finish;
  BiActionState state;
  int duration;
  double progress;
  BiTimer timer;
  BiNode* node;
  void* on_finish_callback_context;
  int repeat; // -1:loop, 0:nop, 1:one-time, 2,3,4...
  int repeat_count;
} BiAction;

extern BiAction* bi_action_init(BiAction *action);
extern void bi_action_start(BiAction *action);
extern void bi_add_action(BiNode* node, BiAction* action);
extern void bi_remove_action(BiNode* node, BiAction* action);
extern void bi_action_set_repeat(BiAction *action,int repeat);

//
// actions
//

// Base

extern void bi_action_base_init(BiAction* action);

// Fade In / Fade Out
typedef struct {
  BiAction action;
  float from;
} BiActionFade;
extern BiActionFade* bi_action_fade_in_init(BiActionFade* fade,int duration);
extern BiActionFade* bi_action_fade_out_init(BiActionFade* fade,int duration);

// Rotate To / Rotate By
typedef struct {
  BiAction action;
  double from;
  double angle;
} BiActionRotate;
extern BiActionRotate* bi_action_rotate_to_init(BiActionRotate* action,int duration,double angle);
extern BiActionRotate* bi_action_rotate_by_init(BiActionRotate* action,int duration,double angle);

// Move To / Move By
typedef struct {
  BiAction action;
  int from_x;
  int from_y;
  int dx;
  int dy;
} BiActionMove;
extern BiActionMove* bi_action_move_to_init(BiActionMove* action,int duration,int dx,int dy);
extern BiActionMove* bi_action_move_by_init(BiActionMove* action,int duration,int dx,int dy);

// Scale To / Scale By
typedef struct {
  BiAction action;
  double from_x;
  double from_y;
  double dx;
  double dy;
} BiActionScale;
extern BiActionScale* bi_action_scale_to_init(BiActionScale* scale,int duration,double dx,double dy);
extern BiActionScale* bi_action_scale_by_init(BiActionScale* scale,int duration,double dx,double dy);

// Sequence
typedef struct {
  BiAction action;
  BiAction* actions[1000];
  int actions_size;
  int progress;
} BiActionSequence;
extern BiActionSequence* bi_action_sequence_init(BiActionSequence* seq,size_t num,BiAction** actions);

#endif
