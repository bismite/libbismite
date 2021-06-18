#ifndef __ARRAY_H__
#define __ARRAY_H__

#include <stdint.h>

typedef struct {
  uint64_t size;
  uint64_t capacity;
  void** objects;
} Array;

void array_init(Array *a);
void array_free(Array *a);
void array_add_object(Array* a, void* object);
void* array_remove_object_at(Array* a, int index);
void* array_remove_object(Array* a, void* object);

#endif
