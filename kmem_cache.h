/*****************************************************************************
 Project: VainOS
 File:    kmem_cache.h

 Description:
   Contains public interface (data structures, function prototypes) for
   memory caches for dynamic objects of same sizes.

  Affected by constants:
    WITHOUT_DEBUG

  Notes:
    -
******************************************************************************/
#ifndef KMEM_CACHE_H_INCLUDED
#define KMEM_CACHE_H_INCLUDED

#ifdef __cplusplus
  extern "C" {
#endif

#define WITHOUT_DEBUG  1
#define WITH_ALLOC_COUNT     0

typedef struct kmem_cache_t {
  que_t         elem;
//  lock_t        lock;
  #if !WITHOUT_DEBUG
  const char    *name;
  #endif
  size_t        item_size;
  unsigned int  items_in_cache;
  unsigned int  max_items;
  void          *cache;
  #if WITH_ALLOC_COUNT
  long          allocated;
  #endif
} kmem_cache_t;

/* macro for new static object creation without need of kmem_cache_init call
   usage:
     kmem_cache_t MyStructs = KMEM_CACHE_INITIALIZER("my struct",sizeof(my_struct_t),100);
*/
#if WITHOUT_DEBUG
#define KMEM_CACHE_INITIALIZER(item_size,max_items)  {{NULL,NULL},item_size,0,max_items,NULL}
#else
#define KMEM_CACHE_INITIALIZER(item_size,max_items)  {{NULL,NULL},name,item_size,0,max_items,NULL}
#endif

/* initialize allocation cache */
void  kmem_cache_init (kmem_cache_t *kmc, const char *name, size_t item_size, unsigned int max_items);

/* allocate chunk */
void *kmem_cache_alloc (kmem_cache_t *kmc);

/* deallocate (free) chunk */
void  kmem_cache_free (kmem_cache_t *kmc, void *object);

/* preallocate chunks */
void  kmem_cache_prealloc (kmem_cache_t *kmc, unsigned int amount);

/* deallocate (free) all allocated, but unused chunks */
void  kmem_cache_purge (kmem_cache_t *kmc);

#ifdef __cplusplus
  }
#endif
#endif
