#ifndef ARRAY_H_INCLUDED
#define ARRAY_H_INCLUDED

#ifdef __cplusplus
  extern "C" {
#endif

typedef struct ARRAY {

  void *D;
  long ElementSize;
  long Size;
  long Used;
  long AllocationStep;

} ARRAY;

#define array_create_index(a,b) array_create_index_i(a,b)

void  array_init (ARRAY *arr, long ElementSize, long AllocationStep);
void *array_append_element (ARRAY *arr);
void  array_delete_data (ARRAY *arr);
void  array_discard_last_element (ARRAY *arr);
long  array_size (ARRAY *arr);
void **array_create_index_i (ARRAY *arr, int (*cmp)(const void *e1, const void *e2));
void **array_create_index_sh (ARRAY *arr, int (*cmp)(const void *e1, const void *e2));

void  array_dump_common (ARRAY *arr, FILE *o);

#ifdef __cplusplus
  }
#endif
#endif
