#ifndef __BI_EXT_ACTION_H__
#define __BI_EXT_ACTION_H__

#include <bi/node.h>
#include <bi/timer.h>

typedef struct _BiAction BiAction;

typedef void (*bi_action_start_function)(BiNode*,BiAction*,double); // target-node,action,now
typedef void (*bi_action_update_function)(BiNode*,BiAction*,double); // target-node,action,rate
typedef void (*bi_action_on_finish_callback)(BiAction*,void*); // action,context

struct _BiAction {
  bi_action_start_function start;
  bi_action_update_function update;
  bi_action_on_finish_callback on_finish;
  bool finit;
  bool started;
  bool finished;
  double start_at;
  double duration;
  void* action_data;
  BiTimer* timer;
  BiNode* node;
  void* on_finish_callback_context;
};

extern void bi_action_init(BiAction *action);
extern void bi_action_start(BiNode *node, BiAction *action,double now);
extern void bi_action_update(BiNode *node, BiAction *action, double rate);

extern void bi_add_action(BiNode* node, BiAction* action);
extern void bi_remove_action(BiNode* node, BiAction* action);

//
// actions
//

// Base

extern void bi_action_base_init(BiAction* action);

// Rotate To

typedef struct {
  int from;
  int to;
} BiActionRotateTo;

extern void bi_action_rotate_to_init(BiAction* action,double duration,double angle);

// Rotate By

typedef struct {
  int from;
  int by;
} BiActionRotateBy;

extern void bi_action_rotate_by_init(BiAction* action,double duration,double angle);

// Move To

typedef struct {
  int from_x;
  int from_y;
  int to_x;
  int to_y;
} BiActionMoveTo;

extern void bi_action_move_to_init(BiAction* action,double duration,int x,int y);

// Sequence

typedef struct {
  BiAction* actions[0xFF]; // XXX: finite
  int actions_size;
} BiActionSequence;

extern void bi_action_sequence_init(BiAction* action,size_t num,BiAction** actions);

// Repeat

typedef struct {
  BiAction* action;
} BiActionRepeat;

extern void bi_action_repeat_init(BiAction* action,BiAction* target);

#endif
