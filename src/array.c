#include <bi/array.h>
#include <stdlib.h>

#define ARRAY_INITIAL_CAPACITY 4

void array_init(Array *a)
{
  a->size = 0;
  a->capacity = 0;
  a->objects = NULL;
  a->sort = NULL;
  a->order_cached = true;
}

void array_free(Array *a)
{
  free(a->objects);
}

void* array_add_object(Array* a, void* object)
{
  a->order_cached = false;
  a->size+=1;
  if(a->size > a->capacity) {
    if(a->capacity==0) {
      a->capacity = ARRAY_INITIAL_CAPACITY;
    }else{
      a->capacity = a->capacity * 2;
    }
    a->objects = realloc( a->objects, sizeof(void*) * a->capacity );
  }
  a->objects[a->size-1] = object;
  return object;
}

void* array_remove_object_at(Array* a, int index)
{
  if(index<0) return NULL;
  if(a->size <= index) return NULL;
  a->order_cached = false;
  void* tmp = a->objects[index];
  for(int i=index; i<a->size-1; i++){
    a->objects[i] = a->objects[i+1];
  }
  a->size -= 1;
  if(a->size==0){
    free(a->objects);
    a->objects = NULL;
    a->capacity = 0;
  }else if(a->size < a->capacity/4){
    a->capacity = a->capacity/2;
    a->objects = realloc(a->objects, sizeof(void*) * a->capacity);
  }

  return tmp;
}

void* array_remove_object(Array* a, void* object)
{
  a->order_cached = false;
  int index = -1;
  for(int i=0; i<a->size; i++){
    if(a->objects[i] == object){
      index = i;
      break;
    }
  }
  if(index<0){
    return NULL;
  }
  return array_remove_object_at(a,index);
}

void array_clear(Array* a)
{
  a->order_cached = true;
  a->capacity = ARRAY_INITIAL_CAPACITY;
  a->objects = realloc( a->objects, sizeof(void*) * a->capacity );
  a->size = 0;
}
