#ifndef __BI_CORE_UTIL_H__
#define __BI_CORE_UTIL_H__

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#else
#include <sys/time.h>
#endif

#include <stdlib.h>
#include <stdint.h>

#ifndef M_PI
#define M_PI (3.14159265358979323846264338327950288)
#endif

static inline int64_t bi_get_now()
{
#ifdef __EMSCRIPTEN__
  return (int64_t)( EM_ASM_DOUBLE({ return Date.now(); }) );
#else
  struct timeval now;
  gettimeofday(&now, NULL);
  return now.tv_sec*1000 + now.tv_usec/1000;
#endif
}

static inline void bi_set_color(uint8_t *color,uint8_t r,uint8_t g,uint8_t b,uint8_t a)
{
  color[0] = r;
  color[1] = g;
  color[2] = b;
  color[3] = a;
}

#endif
