#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "array.h"


/*
  Sedgwick coefficients calculated
  by this function (s = 0..27):

  unsigned long calc_value (unsigned long s)
  {
  unsigned long tmp;
    if (s & 1)
      tmp = 8*(1<<s) - 6*(1<<((s+1)>>1)) + 1;
    else
      tmp = 9*(1<<s) - 9*(1<<(s>>1)) + 1;
    return tmp;
  }

*/
static const long Inc[] = {
1L, 5L, 19L, 41L,
109L, 209L, 505L, 929L,
2161L, 3905L, 8929L, 16001L,
36289L, 64769L, 146305L, 260609L,
587521L, 1045505L, 2354689L, 4188161L,
9427969L, 16764929L, 37730305L, 67084289L,
150958081L, 268386305L, 603906049L, 1073643521L
};

static const unsigned long Inc3[] = {
3UL, 15UL, 57UL, 123UL,
327UL, 627UL, 1515UL, 2787UL,
6483UL, 11715UL, 26787UL, 48003UL,
108867UL, 194307UL, 438915UL, 781827UL,
1762563UL, 3136515UL, 7064067UL, 12564483UL,
28283907UL, 50294787UL, 113190915UL, 201252867UL,
452874243UL, 805158915UL, 1811718147UL, 3220930563UL
};

/******************************************
  Calclulate start index in Sedgwick coeffs
  depending on array size:
******************************************/
static int get_start_inc_index (unsigned long size)
{
int i;
  for (i = 1; i < 28; i++)  {
    if (Inc3[i] > size)  break;
  }
  return (i - 1);
}

/*****************************************************************/
void array_init (ARRAY *arr, long ElementSize, long AllocationStep)
{
  arr->D = NULL;
  arr->ElementSize = ElementSize;
  arr->Size = 0;
  arr->Used = 0;
  arr->AllocationStep = AllocationStep;
}
/*****************************************************************/
void *array_append_element (ARRAY *arr)
{
void *ret = NULL;
long sz, used;

  sz = arr->Size;
  used = arr->Used;

  if (sz > used)  {
    ret = (char *)arr->D + (arr->ElementSize * used);
    arr->Used++;
  }
  else  {
    ret = malloc ((sz + arr->AllocationStep) * arr->ElementSize);
    if (ret)  {
      memcpy (ret, arr->D, used * arr->ElementSize);
      free (arr->D);
      arr->D = ret;
      ret = (char *)arr->D + (arr->ElementSize * used);
      arr->Used++;
      arr->Size += arr->AllocationStep;
    }
  }
  if (ret)  {
    memset (ret, 0, arr->ElementSize);
  }
  return ret;
}

/*****************************************************************/
void array_discard_last_element (ARRAY *arr)
{
  if (arr->Used) arr->Used--;
}

/*****************************************************************/
long array_size (ARRAY *arr)
{
  return arr->Used;
}

/*****************************************************************/
void array_delete_data (ARRAY *arr)
{
  free (arr->D);
  arr->D = NULL;
  arr->Size = 0;
  arr->Used = 0;

}
/*********************************************************************/
void **array_create_index_i (ARRAY *arr, int (*cmp)(const void *e1, const void *e2))
{
void **Index;
void *tmp;
long i, j;

  Index = malloc (arr->Used * sizeof(void *));
  if (Index != NULL)  {
    for (i = 0; i < arr->Used; i++)  {
      Index[i] = arr->D + (i * arr->ElementSize);
    }

    for (i = 0; i < arr->Used; i++)  {
      tmp = Index[i];
      for (j = i - 1; (j >= 0) && ((*cmp)(Index[j], tmp) > 0); j--)  {
        Index[j+1] = Index[j];
      }
      Index[j+1] = tmp;
    }
  }
  return Index;
}
/*********************************************************************/
void **array_create_index_sh (ARRAY *arr, int (*cmp)(const void *e1, const void *e2))
{
void **Index;
void *tmp;
long i, j, inc;
int incIdx;

  Index = malloc (arr->Used * sizeof(void *));
  if (Index != NULL)  {
    for (i = 0; i < arr->Used; i++)  {
      Index[i] = arr->D + (i * arr->ElementSize);
    }

    incIdx = get_start_inc_index (arr->Used);
    while (incIdx >= 0)  {
      inc = Inc[incIdx--];  // set next step (Sedgwick algo)
      for (i = inc; i < arr->Used; i++)  {
        tmp = Index[i];
        for (j = i-inc; (j >= 0) && ((*cmp)(Index[j], tmp) > 0); j-=inc)  {
          Index[j+inc] = Index[j];
        }
        Index[j+inc] = tmp;
      }
    }
  }
  return Index;
}



/*****************************************************************/
void array_dump_common (ARRAY *arr, FILE *o)
{
  fprintf (o, "array size = %ld\n", arr->Size);
  fprintf (o, "array used = %ld\n", arr->Used);
}





/*****************************************************************/

#ifdef TEST

#include <stdio.h>

typedef struct m_t {
  char aaa[10];
  long bbb;
} m_t;

ARRAY a;
m_t **idx;

void dump (void)
{
long i;
m_t *data;

  data = a.D;
  for (i = 0; i < a.Used; i++)  {
    printf ("%ld. %s %ld\n", i, data[i].aaa, data[i].bbb);
  }
}

void dump_idx (void)
{
long i;
m_t *data;

  for (i = 0; i < a.Used; i++)  {
    data = idx[i];
    printf ("%ld. %s %ld\n", i, data->aaa, data->bbb);
  }
}


int ccc (void *e1, void *e2)
{
m_t *m1 = e1;
m_t *m2 = e2;
  return m1->bbb - m2->bbb;
}

int main (void)
{
m_t *p;

  array_init (&a, sizeof(m_t), 2);
  p = array_append_element (&a);
  p->bbb = 5;
  strcpy (p->aaa, "e1");
  p = array_append_element (&a);
  p->bbb = 46;
  strcpy (p->aaa, "e2");
  p = array_append_element (&a);
  p->bbb = 7;
  strcpy (p->aaa, "e3");
  p = array_append_element (&a);
  p->bbb = 339;
  strcpy (p->aaa, "e4");
  p = array_append_element (&a);
  p->bbb = 125;
  strcpy (p->aaa, "e5");

  idx = (m_t**) array_create_index (&a, ccc);

  dump ();
  printf ("-----------\n");
  dump_idx ();


  array_delete_data (&a);
}

#endif
