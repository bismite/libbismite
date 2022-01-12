#ifndef __BISMITE_UTIL_H__
#define __BISMITE_UTIL_H__

#include <sys/time.h>
#include <stdlib.h>
#include <stdint.h>

#ifndef M_PI
#define M_PI (3.14159265358979323846264338327950288)
#endif

static inline int64_t bi_get_now()
{
  struct timeval now;
  gettimeofday(&now, NULL);
  return now.tv_sec*1000 + now.tv_usec/1000;
}

static inline void bi_set_color(uint8_t *color,uint8_t r,uint8_t g,uint8_t b,uint8_t a)
{
  color[0] = r;
  color[1] = g;
  color[2] = b;
  color[3] = a;
}

#endif
