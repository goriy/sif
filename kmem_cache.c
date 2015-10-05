/*****************************************************************************
 Project: VainOS
 File:    kmem_cache.c

 Description:
   Содержит реализацию кеша динамических объектов.

  Affected by constants:
    WITHOUT_DEBUG

  Notes:
    Если кеш динамических объектов не используются, то можно этот файл
    в рабочий проект не включать.
******************************************************************************/
#include <stdlib.h>
#include "que.h"
#include "kmem_cache.h"

#define lock_pend(a,b)
#define lock_post(a)
#define lock_init_ex(a,b)
#define OTXT(a)  ""
#define unlikely(x)  (x)
#define os_alloc(s,t)   malloc(s)
#define os_free(p)      free(p)

typedef struct kmem_cache_item_t  {
  struct kmem_cache_item_t *next;
} kmem_cache_item_t;

/***************************************************************/
void  kmem_cache_init (kmem_cache_t *kmc, const char *name, size_t item_size, unsigned int max_items)
{
  if (unlikely ((size_t)item_size < sizeof (kmem_cache_item_t)))  {
    item_size = sizeof (kmem_cache_item_t);
  }
  kmc->cache = NULL;
  kmc->item_size = item_size;
  kmc->max_items = max_items;
  kmc->items_in_cache = 0;
  #if WITH_ALLOC_COUNT
  kmc->allocated = 0;
  #endif
  lock_init_ex (&kmc->lock, name);
  #if !WITHOUT_DEBUG
  kmc->name = name;
  #endif
}

/***************************************************************/
void *kmem_cache_alloc (kmem_cache_t *kmc)
{
kmem_cache_item_t *item;

  lock_pend (&kmc->lock, SYS_FOREVER);
  #if WITH_ALLOC_COUNT
  kmc->allocated++;
  #endif
  if (kmc->items_in_cache > 0)  {
    item = (kmem_cache_item_t *)kmc->cache;
    kmc->cache = item->next;
    kmc->items_in_cache--;
    lock_post (&kmc->lock);
  }
  else  {
    lock_post (&kmc->lock);
    item = os_alloc (kmc->item_size, OTXT(kmc->name));
  }
  return (void *)item;
}

/***************************************************************/
void kmem_cache_free (kmem_cache_t *kmc, void *object)
{
kmem_cache_item_t *item;

  if (unlikely (object == NULL)) return;

  lock_pend (&kmc->lock, SYS_FOREVER);
  #if WITH_ALLOC_COUNT
  kmc->allocated--;
  #endif
  if (kmc->items_in_cache < kmc->max_items)  {
    item = (kmem_cache_item_t *)object;
    item->next = (kmem_cache_item_t *)kmc->cache;
    kmc->cache = item;
    kmc->items_in_cache++;
    lock_post (&kmc->lock);
  }
  else  {
    lock_post (&kmc->lock);
    os_free (object);
  }
}

/***************************************************************/
void kmem_cache_prealloc (kmem_cache_t *kmc, unsigned int amount)
{
kmem_cache_item_t *item;
unsigned int i;

  lock_pend (&kmc->lock, SYS_FOREVER);
  if (unlikely (amount > kmc->max_items)) amount = kmc->max_items;
  for (i = 0; i < amount; i++)  {
    item = (kmem_cache_item_t *)os_alloc (kmc->item_size, OTXT(kmc->name));
    kmem_cache_free (kmc, item);
  }
  lock_post (&kmc->lock);
}

/***************************************************************/
void kmem_cache_purge (kmem_cache_t *kmc)
{
kmem_cache_item_t *item;

  lock_pend (&kmc->lock, SYS_FOREVER);
  while (kmc->items_in_cache > 0)  {
    item = (kmem_cache_item_t *)kmc->cache;
    kmc->cache = item->next;
    kmc->items_in_cache--;
    os_free (item);
  }
  lock_post (&kmc->lock);
}
