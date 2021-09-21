#ifndef __BI_EXT_SIMPLE_UNICODE_H__
#define __BI_EXT_SIMPLE_UNICODE_H__

#include <stdint.h>
#include <stdlib.h>

extern uint16_t utf8_getch_as_ucs2(const char **src, size_t *srclen);

extern uint16_t utf8_to_ucs2(uint32_t utf8);

extern int utf8_get_character(const char *src, char *dst);

#endif
