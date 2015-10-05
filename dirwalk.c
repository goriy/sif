/*

 To build test you should run:

 gcc dirwalk.c -D DWALK_TEST -o dirwalk_test.exe


*/

#include <stdio.h>
#include <string.h>
#include <io.h>
#include <stdlib.h>

#include "dirwalk.h"


#define MASK_USED


#ifdef MASK_USED
#include "mask.h"
#endif

/* ======================================================= */

static void *emalloc (size_t len)
{
void *p;

    p = malloc (len);
    if (p == NULL)  {
      fprintf (stderr, "Not enough memory\n");
      exit (1);
    }
    return p;
}

/* ======================================================= */

static int get_contents (const char *path, const char *filter, int depth, int recurse, DWALK_FUNC ffunc, DWALK_FUNC dfunc)
{
long id;
struct _finddata_t f;
char *npath;
int res;

    #ifdef MASK_USED
    npath = (char *)malloc(strlen(path)+3+3);
    strcpy (npath, path);
    strcat (npath, DIR_SEPARATOR);
    strcat (npath, "*");
    #else
    npath = (char *)malloc(strlen(path)+strlen(filter)+3);
    strcpy (npath, path);
    strcat (npath, DIR_SEPARATOR);
    strcat (npath, filter);
    #endif

    id = _findfirst (npath, &f);

    free (npath);

    if (id == -1) {
        return 0;
    }

    do  {
        if (!strcmp (f.name, "."))  continue;
        if (!strcmp (f.name, "..")) continue;

        if (f.attrib & _A_SUBDIR)  {
          if (recurse)  {
/*
//              printf ("%08d\td\t%s/%s\n", depth, path,f.name);
//              fprintf (stderr, "%s/%s\n", path,f.name);
*/
            #ifdef MASK_USED
            npath = (char *)malloc(strlen(path)+5+strlen(f.name));
            strcpy (npath, path);
            strcat (npath, DIR_SEPARATOR);
            strcat (npath, f.name);
            #else
            npath = (char *)malloc(strlen(path)+5+strlen(f.name));
            strcpy (npath, path);
            strcat (npath, DIR_SEPARATOR);
            strcat (npath, f.name);
            #endif
            res = 1;
            if (dfunc != NULL)  {
              res = dfunc (npath, f.name, &f, depth);
            }

            if (res > 0)  {
              res = get_contents (npath, filter,depth+1,recurse,ffunc,dfunc);
            }

            free (npath);
            if (!res) {return 0;}
          }
        }
        else    {
          if (ffunc != NULL)  {
            #ifdef MASK_USED
            if (mask_match (f.name, filter))  {
/*              printf ("  [OK]\n");*/
              res = ffunc (path, f.name, &f, depth);
              if (!res) return 0;
            }
/*            else printf ("  [FAIL]\n");*/
            #else
            res = ffunc (path, f.name, &f, depth);
            if (!res) return 0;
            #endif
          }
/*          printf ("%08d\tf\t%s/%s\n", depth, path, f.name);*/
        }
    } while (_findnext(id, &f) == 0);

    return 1;
}

/* ======================================================= */

void dirwalk (const char *path, const char *filter, int recurse, DWALK_FUNC ffunc, DWALK_FUNC dfunc)
{
    get_contents (path,filter,0,recurse,ffunc,dfunc);
}

/* ======================================================= */

void path_mask (char *full, char **path, char **mask)
{
char *p, *m;
int lfull, i, k;

  lfull = strlen (full);

  p = emalloc (lfull > 1 ? lfull+1 : 2);
  m = emalloc (lfull > 3 ? lfull+1 : 4);

  if (lfull == 0)  {
    strcpy (p, ".");
    strcpy (m, "*.*");
    *path = p;
    *mask = m;
    return;
  }

  strcpy (p, full);

  i = lfull - 1;
  k = i;
  while (i)  {
    if ((p[i] == '/') || (p[i] == '\\'))  {
      k = i;
      p[i] = 0;
      break;
    }
    i--;
  }

  if (i == 0)  {
    if ((strcmp(full,".") == 0) || (strcmp(full,"..") == 0))  {
      strcpy (m, "*.*");
    }
    else  {
      strcpy (p, ".");
      strcpy (m, full);
    }
  }
  else if (k == (lfull - 1))  {
    strcpy (m, "*.*");
  }
  else  {
    strcpy (m, full+k+1);
  }

  *path = p;
  *mask = m;
}

#ifdef DWALK_TEST


int test_path_mask (void)
{
char *path, *mask, *res;

  /***************/

  path_mask ("c:\\windows\\temp\\", &path, &mask);
  res = " [FAIL]";
  if ((strcmp (path, "c:\\windows\\temp") == 0) && (strcmp (mask, "*.*") == 0))  {
    res = " [OK]";
  }
  printf ("%s -> %s   %s\n", path, mask, res);
  free (path);
  free (mask);

  /***************/

  path_mask (".", &path, &mask);
  res = " [FAIL]";
  if ((strcmp (path, ".") == 0) && (strcmp (mask, "*.*") == 0))  {
    res = " [OK]";
  }
  printf ("%s -> %s   %s\n", path, mask, res);
  free (path);
  free (mask);

  /***************/

  path_mask ("..", &path, &mask);
  res = " [FAIL]";
  if ((strcmp (path, "..") == 0) && (strcmp (mask, "*.*") == 0))  {
    res = " [OK]";
  }
  printf ("%s -> %s   %s\n", path, mask, res);
  free (path);
  free (mask);

  /***************/

  path_mask ("./", &path, &mask);
  res = " [FAIL]";
  if ((strcmp (path, ".") == 0) && (strcmp (mask, "*.*") == 0))  {
    res = " [OK]";
  }
  printf ("%s -> %s   %s\n", path, mask, res);
  free (path);
  free (mask);

  /***************/

  path_mask ("./test/dir*.c*", &path, &mask);
  res = " [FAIL]";
  if ((strcmp (path, "./test") == 0) && (strcmp (mask, "dir*.c*") == 0))  {
    res = " [OK]";
  }
  printf ("%s -> %s   %s\n", path, mask, res);
  free (path);
  free (mask);

  /***************/

  path_mask ("dir*.c*", &path, &mask);
  res = " [FAIL]";
  if ((strcmp (path, ".") == 0) && (strcmp (mask, "dir*.c*") == 0))  {
    res = " [OK]";
  }
  printf ("%s -> %s   %s\n", path, mask, res);
  free (path);
  free (mask);

  /***************/

  return 1;
}

/*
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <io.h>
*/

int prt_f (char *path, char *name, int depth)
{
  printf ("depth=%4d\tfile\t%s  ====> name: %s\r\n", depth, path, name);
  return 1;
}

int prt_d (char *path, char *name, int depth)
{
/*  printf ("depth=%4d\tdir \t%s  ====> name: %s\r\n", depth, path, name);*/
  return -1;
}


int main (int argc, char *argv[])
{
char *path, *mask;
int i;

    printf ("argument list:\n");
    for (i = 0; i < argc; i++)  {
      printf ("%d. %s\n", i, argv[i]);
    }
    printf ("=============\n");

    if (argc == 1)  {
      path = ".";
      mask = "*.*";
    }
    else if (argc == 2)  {
      path_mask (argv[1], &path, &mask);
    }
    else  {
      path = argv[1];
      mask = argv[2];
    }

    printf ("path = %s\n", path);
    printf ("mask = %s\n=============\n", mask);

    dirwalk (path, mask, 1, prt_f, prt_d);

    test_path_mask ();

    return 0;
}

#endif
