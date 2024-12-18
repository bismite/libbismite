#ifndef __ARRAY_H__
#define __ARRAY_H__

#include <stdint.h>
#include <stdbool.h>

typedef struct _Array Array;
struct _Array {
  uint64_t size;
  uint64_t capacity;
  void** objects;
  bool order_cached;
  void (*sort)(Array*);
};

void array_init(Array *a);
void array_clear(Array* a);
void* array_add_object(Array* a, void* object);
int array_object_index(Array* a, void* object);
static inline void* array_object_at(Array* a, int index){ return a->objects[index]; }
void* array_remove_object_at(Array* a, int index);
void* array_remove_object(Array* a, void* object);
static inline void array_sort(Array* a){ if(a->sort) a->sort(a); }

#endif
