#ifndef __BI_IMAGE_H__
#define __BI_IMAGE_H__

#include <stdint.h>

extern void bi_image_rgba32_flip_vertical(int w, int h, uint8_t* pixels, uint8_t* flipped);
extern void bi_image_rgba32_save_png(int w, int h, uint8_t* pixels, const char* filename);

#endif
