#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "ini.h"
#include "mstr.h"

static INI_VALUE *ini_search_value        (const INI *ini, const char *section, const char *key);
static INI_VALUE *ini_search_value_linear (const INI *ini, const char *section, const char *key);
static INI_VALUE *ini_search_value_bin    (const INI *ini, const char *section, const char *key);
static int ini_add (INI *d, const char *section, const char *key, const char *value);
static int ini_load_string (INI *d, const char *section, char *buf, char separator);

/***********************************/
INI *ini_create (long ItemsToAllocate)
{
INI *d;
  d = malloc (sizeof(INI));
  if (NULL != d)  {
    ini_init (d, ItemsToAllocate);
  }
  return d;
}

/***********************************/
void ini_init (INI *d, long ItemsToAllocate)
{
  array_init (&d->a, sizeof(INI_VALUE), ItemsToAllocate);
  d->Index = NULL;
}

/************************************/
void ini_clear (INI *d)
{
  array_delete_data (&d->a);
  if (d->Index)  free (d->Index);
}

/************************************/
void ini_delete (INI *d)
{
  ini_clear (d);
  free (d);
}

/***********************************/
static int ini_add (INI *d, const char *section, const char *key, const char *value)
{
char *k, *v, *s;
INI_VALUE *p;
int slen = strlen (section);
int klen = strlen (key);
int vlen = strlen (value);

  k = malloc (klen+1);
  if (NULL == k)  {
    return 1;
  }
  v = malloc (vlen+1);
  if (NULL == v)  {
    free (k);
    return 1;
  }
  s = malloc (slen+1);
  if (NULL == s)  {
    free (v);
    free (k);
    return 1;
  }

  strcpy (s, section);
  strcpy (k, key);
  strcpy (v, value);

  if (d->Index)  free (d->Index);

  p = (INI_VALUE *) array_append_element (&d->a);
  if (NULL == p)  {
    free (v);
    free (k);
    free (s);
    return 1;
  }

  p->key = k;
  p->value = v;
  p->section = s;
  return 0;
}

/***********************************/
char *ini_search (const INI *d, const char *section, const char *key, char *defvalue)
{
char *res = defvalue;
INI_VALUE *p;

  p = ini_search_value (d, section, key);
  if (p)  {
    res = p->value;
  }
  return res;
}

/***********************************/
unsigned long ini_search_ulong (const INI *d, const char *section, const char *key, unsigned long defvalue)
{
unsigned long res = defvalue;
INI_VALUE *p;

  p = ini_search_value (d, section, key);
  if (p)  {
    res = ustrtoul (p->value, NULL);
  }
  return res;
}

/***********************************/
long ini_search_long (const INI *d, const char *section, const char *key, long defvalue)
{
long res = defvalue;
INI_VALUE *p;

  p = ini_search_value (d, section, key);
  if (p)  {
    res = ustrtol (p->value, NULL);
  }
  return res;
}

/***********************************/
double ini_search_double (const INI *d, const char *section, const char *key, double defvalue)
{
double res = defvalue;
INI_VALUE *p;

  p = ini_search_value (d, section, key);
  if (p)  {
    res = strtod (p->value, NULL);
  }
  return res;
}

/************************************/
static int ini_modify (INI_VALUE *p, const char *NewVal)
{
char *v;
int vlen = strlen (NewVal);
  v = malloc (vlen+1);
  if (NULL == v)  {
    return 1;
  }
  strcpy (v, NewVal);
  free (p->value);
  p->value = v;
  return 0;
}
/************************************/
int ini_add_or_modify (INI *d, const char *section, const char *key, const char *value)
{
INI_VALUE *p = ini_search_value (d, section, key);

  if (p)  {
    return ini_modify (p, value);
  }
  else  {
    return ini_add (d, section, key, value);
  }
}

/************************************/
void ini_dump (INI *d, FILE *o, const char *prefix)
{
int i;
INI_VALUE *p = d->a.D;
/*
  fprintf (o, "------------ ini dump ------------\n");
  array_dump_common (&d->a, o);
*/
  for (i = 0; i < d->a.Used; i++)  {
    fprintf (o, "%s[%s]/%s = %s\n", prefix, p[i].section, p[i].key, (char *)p[i].value);
  }
}
/************************************/
static int ini_load_string (INI *d, const char *section, char *buf, char separator)
{
char *p;
int ret = 0;

  p = strchr (buf, separator);
  if (p)  {
    *p = 0;
    p++;
    trim_string (buf);
    trim_string (p);
    ret = ini_add_or_modify (d, section, buf, p);
  }
  return ret;
}

/******************************************************/
int ini_load_file (INI *ini, const char *filename)
{
FILE *in;
char buf[512];
int ret = 0;
char section[128];

  in = fopen (filename, "r");
  section[0] = 0;
  if (NULL == in)  {
    fprintf (stderr, "Unable to open config file: %s\n", filename);
    return 1;
  }

  while (!read_line(in, buf, sizeof(buf)))  {
    if (buf[0] == '[')  {
      char *p;
      strcpy (section, buf + 1);
      p = strchr (section, ']');
      if (p)  *p = 0;
      trim_string (section);
    }
    else  {
      ini_load_string (ini, section, buf, '=');
    }
  }
  fclose (in);
  return ret;
}

/******************************************************/
static int cmp_ini_value (const void *e1, const void *e2)
{
const INI_VALUE *v1 = (const INI_VALUE *)e1, *v2 = (const INI_VALUE *)e2;
int res;
  res = strcmp (v1->section, v2->section);
  if (!res)  {
    res = strcmp (v1->key, v2->key);
  }
  return res;
}
/******************************************************/
static int cmp_ini_value_and_data (const INI_VALUE *e1, const char *section, const char *key)
{
int res;
  res = strcmp (e1->section, section);
  if (!res)  {
    res = strcmp (e1->key, key);
  }
  return res;
}
/******************************************************/
void ini_create_index (INI *ini)
{
  ini->Index = (INI_VALUE**) array_create_index_sh (&ini->a, cmp_ini_value);
}

/******************************************************/
static INI_VALUE *ini_search_value (const INI *ini, const char *section, const char *key)
{
  if (ini->Index)  {
    return ini_search_value_bin (ini, section, key);
  }
  else  {
    return ini_search_value_linear (ini, section, key);
  }
}

/******************************************************/
static INI_VALUE *ini_search_value_linear (const INI *ini, const char *section, const char *key)
{
int i;
INI_VALUE *p = ini->a.D;
  for (i = 0; i < ini->a.Used; i++)  {
    if (!cmp_ini_value_and_data (&p[i], section, key))  {
      return &p[i];
    }
  }
  return NULL;
}

/*********************************************************************/
static INI_VALUE *ini_search_value_bin (const INI *ini, const char *section, const char *key)
{
long minI, maxI, step, i;
long res;

  minI = 0;
  maxI = ini->a.Used;
  step = maxI;


  while (step > 0)  {
    i = (maxI + minI) >> 1;
    res = cmp_ini_value_and_data (ini->Index[i], section, key);
    if (res == 0)  {
      return ini->Index[i];
    }
    else if (res > 0)  {
      maxI = i;
      step >>= 1;
    }
    else  {
      minI = i;
      step >>= 1;
    }
  }
  return NULL;
}



/*********************************************************************/
#ifdef SELFTEST
INI Cfg;

int main (int argc, char *argv[])
{
int i;

  if (argc < 2) return 1;
  ini_init (&Cfg, 100);
  ini_load_file (&Cfg, argv[1]);
  //ini_dump (&Cfg, stdout, "");
  ini_create_index (&Cfg);
  printf ("VALUE = %s\n", ini_search (&Cfg, "PHP", "post_max_size", "no value"));
  printf ("VALUE = %s\n", ini_search (&Cfg, "ldap", "ldap.max_links", "no value"));

  for (i = 0; i < 1000000UL; i++)  {
    ini_search (&Cfg, "PHP", "post_max_size", "no value");
    ini_search (&Cfg, "ldap", "ldap.max_links", "no value");
  }

  ini_clear (&Cfg);
  return 0;
}

#endif
