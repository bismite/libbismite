#ifndef __BISMITE_COLOR_H__
#define __BISMITE_COLOR_H__

#include <stdint.h>

typedef struct BiColor {
  uint8_t r;
  uint8_t g;
  uint8_t b;
  uint8_t a;
} BiColor;

inline static BiColor RGBA(uint8_t r,uint8_t g, uint8_t b,uint8_t a){
  BiColor c;
  c.r = r;
  c.g = g;
  c.b = b;
  c.a = a;
  return c;
}

inline static BiColor RGBA32(uint32_t rgba){
  BiColor c;
  c.r = rgba >> 24 & 0xFF;
  c.g = rgba >> 16 & 0xFF;
  c.b = rgba >>  8 & 0xFF;
  c.a = rgba >>  0 & 0xFF;
  return c;
}

#endif
