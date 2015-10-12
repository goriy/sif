#ifndef INI_H_INCLUDED
#define INI_H_INCLUDED

#include "array.h"

#ifdef __cplusplus
  extern "C" {
#endif

typedef struct INI_VALUE  {
  char *section;
  char *key;
  char *value;
} INI_VALUE;

typedef struct INI  {
  ARRAY  a;
  INI_VALUE **Index;
} INI;

/* create/delete functions */
INI *ini_create (long ItemsToAllocate);
void ini_delete (INI *d);

void ini_init  (INI *d, long ItemsToAllocate);
void ini_clear (INI *d);

/* add data */
int ini_add_or_modify (INI *d, const char *section, const char *key, const char *value);
int ini_load_file (INI *ini, const char *filename);

/* index */
void ini_create_index (INI *ini);

/* search */
char *        ini_search        (const INI *d, const char *section, const char *key, char *defvalue);
unsigned long ini_search_ulong  (const INI *d, const char *section, const char *key, unsigned long defvalue);
long          ini_search_long   (const INI *d, const char *section, const char *key, long defvalue);
double        ini_search_double (const INI *d, const char *section, const char *key, double defvalue);

/* debug */
void ini_dump (INI *d, FILE *o, const char *prefix);


#ifdef __cplusplus
  }
#endif


#endif
