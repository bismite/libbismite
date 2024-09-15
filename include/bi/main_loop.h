#ifndef __BI_CORE_MAIN_LOOP_H__
#define __BI_CORE_MAIN_LOOP_H__

typedef struct _BiContext BiContext;

extern void bi_start_run_loop(BiContext* context);
extern void bi_stop_run_loop();

#endif
