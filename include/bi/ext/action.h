#ifndef __BI_EXT_ACTION_H__
#define __BI_EXT_ACTION_H__

#include <bi/node.h>
#include <bi/timer.h>

typedef struct _BiAction BiAction;

typedef void (*bi_action_start_function)(BiAction*); // action
typedef void (*bi_action_update_function)(BiAction*,double,int); // action,rate,delta_time
typedef void (*bi_action_on_finish_callback)(BiAction*,void*); // action,context

typedef enum {
  BI_ACTION_STATE_READY,
  BI_ACTION_STATE_RUNNING,
  BI_ACTION_STATE_FINISHED
} BiActionState;

struct _BiAction {
  bi_action_start_function start;
  bi_action_update_function update;
  bi_action_on_finish_callback on_finish;
  BiActionState state;
  int duration;
  double progress;
  void* action_data;
  BiTimer timer;
  BiNode* node;
  void* on_finish_callback_context;
};

extern void bi_action_init(BiAction *action);
extern void bi_action_start(BiAction *action);
extern void bi_add_action(BiNode* node, BiAction* action);
extern void bi_remove_action(BiNode* node, BiAction* action);

//
// actions
//

// Base

extern void bi_action_base_init(BiAction* action);

// Fade In / Fade Out
typedef struct {
  int from;
} BiActionFade;
extern void bi_action_fade_in_init(BiAction* action,int duration);
extern void bi_action_fade_out_init(BiAction* action,int duration);

// Rotate To / Rotate By
typedef struct {
  double from;
  double angle;
} BiActionRotate;
extern void bi_action_rotate_to_init(BiAction* action,int duration,double angle);
extern void bi_action_rotate_by_init(BiAction* action,int duration,double angle);

// Move To / Move By
typedef struct {
  int from_x;
  int from_y;
  int dx;
  int dy;
} BiActionMove;
extern void bi_action_move_to_init(BiAction* action,int duration,int dx,int dy);
extern void bi_action_move_by_init(BiAction* action,int duration,int dx,int dy);

// Scale To / Scale By
typedef struct {
  double from_x;
  double from_y;
  double dx;
  double dy;
} BiActionScale;
extern void bi_action_scale_to_init(BiAction* action,int duration,double dx,double dy);
extern void bi_action_scale_by_init(BiAction* action,int duration,double dx,double dy);

// Sequence
typedef struct {
  BiAction* actions[0xFF]; // XXX: finite
  int actions_size;
  int progress;
} BiActionSequence;
extern void bi_action_sequence_init(BiAction* action,size_t num,BiAction** actions);

// Repeat
typedef struct {
  BiAction* action;
  int cursor;
} BiActionRepeat;
extern void bi_action_repeat_init(BiAction* action,BiAction* target);

#endif
