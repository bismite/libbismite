#include <bi/array.h>
#include <stdlib.h>

void array_init(Array *a)
{
  a->size = 0;
  a->capacity = 0;
  a->objects = NULL;
}

void array_free(Array *a)
{
  free(a->objects);
}

void array_add_object(Array* a, void* object)
{
  a->size+=1;
  if(a->size > a->capacity) {
    if(a->capacity==0) {
      a->capacity = 4; // initial capacity
    }else{
      a->capacity = a->capacity * 2;
    }
    a->objects = realloc( a->objects, sizeof(void*) * a->capacity );
  }
  a->objects[a->size-1] = object;
}

void* array_remove_object_at(Array* a, int index)
{
  if(index<0) return NULL;
  if(a->size <= index) return NULL;

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
