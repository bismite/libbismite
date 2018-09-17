#ifndef __BI_CORE_LOGGER_H__
#define __BI_CORE_LOGGER_H__

#include <bi/bi_sdl.h>
#include <stdio.h>

#ifdef __EMSCRIPTEN__
#define LOG(format, ...) printf(format, ##__VA_ARGS__)
#else
#define LOG(format, ...) SDL_Log(format, ##__VA_ARGS__)
#endif

#endif
