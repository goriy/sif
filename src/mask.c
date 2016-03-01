#include <windows.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include "config.h"
#include "mask.h"
#include "mstr.h"

#define PC_TEST 0

void ucc (char *s)
{
int ch;

  while ((ch = (*s)))  {
    if      ((ch >= 'a') && (ch <= 'z'))    *s++ = ch - 0x20;
    else if ((ch >= 0xE0) && (ch <= 0xFF))  *s++ = ch - 0x20;
    else if (ch == 0xB8)  *s++ = 0xA8;  // letter YO in cp1251 encoding
    else *s++ = toupper(ch);
  }
}

/*****************************************************************************/

static char *get_pattern_from_pos (char *source, int slen, int pos)
{
int i, dlen;
char ch;
char *dest, *ptmp;

  dest = (char *)malloc (slen + 1);
  if (dest == NULL)  {
    exit(1);
  }
  dest[slen] = 0;
  ptmp = dest;

  *ptmp++ = source[pos];
  dlen = 1;

  for (i = pos + 1; i < slen; i++)  {
    ch = source[i];
    if ((ch == '*') || (ch == '?'))  {
      break;
    }
    else  {
      *ptmp++ = ch;
      dlen++;
    }
  }

  dest[dlen] = 0;
  return dest;
}

/*****************************************************************************/
static int is_there_any_sym_in_mask(char *mask, int slen, int pos)
{
int i;
char ch;

  for (i = pos; i < slen; i++)  {
    ch = mask[i];
    if (ch != '*') return 1;
  }
  return 0;
}

/*****************************************************************************/
int mask_match (const char *expr, const char *mask)
{
int sourceLen, searchLen;
int cSource = 0, cSearch = 0;
char curSourceSym, curSearchSym;
int FreeSearchFlag = 0;
int res = 0;
char *tmpStr;

char *mexpr, *mmask;


  sourceLen = strlen(expr);
  searchLen = strlen(mask);

  if (searchLen == 0)  {
    return 1;
  }

  mexpr = (char *)malloc (sourceLen + 1);
  if (mexpr == NULL) {
    return 0;
  }
  mmask = (char *)malloc (searchLen + 1);
  if (mmask == NULL)  {
    free (mexpr);
    return 0;
  }

  strcpy (mexpr, expr);
  strcpy (mmask, mask);

  ucc (mexpr);
  ucc (mmask);

  curSourceSym = *mexpr;
  curSearchSym = *mmask;

  while ((cSource < sourceLen) && (cSearch < searchLen))  {

    curSourceSym = mexpr[cSource];
    curSearchSym = mmask[cSearch];

    if (curSearchSym == '?')  {
      cSource++;
      cSearch++;
    }
    else if (curSearchSym == '*')  {
      cSearch++;
      FreeSearchFlag = 1;
    }
    else  {
      if (FreeSearchFlag)  {
        char *tstr;
        int  tlen;

        tmpStr = get_pattern_from_pos(mmask, searchLen, cSearch);
        tlen = strlen(tmpStr);
        tstr = strstr(mexpr + cSource, tmpStr);
        free (tmpStr);
        if (tstr == NULL)  {
          free (mexpr);
          free (mmask);
          return 0;
        }
        else  {
          cSource = (tstr - mexpr) + tlen;
          cSearch += tlen;
          FreeSearchFlag = 0;
        }
      }
      else  {
        if (curSourceSym != curSearchSym)  {
          free (mexpr);
          free (mmask);
          return 0;
        }
        else  {
          cSource++;
          cSearch++;
        }
      }
    }
  }

  if (cSource >= sourceLen) {
    if (cSearch >= searchLen)  {
      res = 1;
    }
    else  {
      curSearchSym = mmask[cSearch];
      if (is_there_any_sym_in_mask(mmask, searchLen, cSearch)) res = 0;
      else                                                     res = 1;
    }
  }
  else  {
    if (cSearch >= searchLen)   {
      if (curSearchSym != '*')  res = 0;
      else                      res = 1;
    }
  }

  free (mexpr);
  free (mmask);
  return res;
}

/*****************************************************************************/
int masks_match (const char *expr, mask_t *m)
{
int i, ret;
  if ((m->tokens == NULL) || (m->cnt < 1))  {
    ret = mask_match (expr, "*");
  }
  else  {
    for (i = 0; i < m->cnt; i++)  {
      ret = mask_match (expr, m->tokens[i]);
      if (ret)  {
        break;
      }
    }
  }
  return ret;
}

/*****************************************************************************/
#define ALLOC_STEP  16

static int token_add (char *token, mask_t *m)
{
char **p;
int ret = 0;
  if (m->cnt >= m->allocated)  {
    p = (char **)realloc(m->tokens, (m->allocated+ALLOC_STEP)*sizeof(char*));
    if (p == NULL)  {
      masks_unprepare (m);
      ret = 1;
    }
    else  {
      m->tokens = p;
      m->allocated += ALLOC_STEP;
      m->tokens[m->cnt++] = trim_string(token);
      // if leading & trailing spaces are a valid symbols in mask
      //m->tokens[m->cnt++] = token;
    }
  }
  else  {
    m->tokens[m->cnt++] = trim_string(token);
    // if leading & trailing spaces are a valid symbols in mask
//    m->tokens[m->cnt++] = token;
  }
  return ret;
}

/*****************************************************************************/
int masks_prepare (const char *mask, mask_t *m)
{
char *tok;
int ret = 0;
  m->cnt = 0;
  m->allocated = 0;
  strncpy (m->bulk, mask, MAX_MASK_LEN-1);
  m->bulk[MAX_MASK_LEN-1] = 0;
  m->tokens = NULL;

  tok = strtok (m->bulk, ";,");
  while (tok != NULL)  {
    if (!m_is_space_string (tok))  {
      ret = token_add (tok, m);
      if (ret)  {
        break;
      }
    }
    tok = strtok (NULL, ";,");
  }
  return ret;
}

/*****************************************************************************/
void masks_unprepare (mask_t *m)
{
  free (m->tokens);
  m->tokens = NULL;
  m->allocated = 0;
  m->cnt = 0;
}



#if PC_TEST

static int totalTests   = 0;
static int successTests = 0;

/* returns 1 if correct */
int do_test (const char *expr, const char *pattern, int MustMatch)
{
int res = mask_match (expr, pattern);
int i, len;

  totalTests++;

  len = printf ("%c[%c] '%s' - '%s' ", res ? '+' : '-', MustMatch ? '+' : '-', expr, pattern);
  for (i = len; i < 70; i++)  printf (".");
  if (res == MustMatch)  {
    printf ("[  OK  ]\n");
    successTests++;
    return 1;
  }
  else  {
    printf ("[ FAIL ]\n");
    return 0;
  }
}

int main (void)
{

  do_test ("adm3202", "*32*"       , 1);
  do_test ("adm3202", "*33*"       , 0);
  do_test ("adm3202", "a*32*2"     , 1);
  do_test ("adm3202", "*a"         , 0);
  do_test ("adm3202", "d*"         , 0);
  do_test ("adm3202", ""           , 1);
  do_test ("adm3202", "*"          , 1);
  do_test ("adm3202", "*A***32****", 1);
  do_test ("", ""                  , 1);
  do_test ("aab", ""               , 1);
  do_test ("", "?"                 , 0);
  do_test ("", "*"                 , 1);
  do_test ("", "***"               , 1);
  do_test ("", "a"                 , 0);
  do_test ("", "*a*"               , 0);

  printf ("\nSuccessful tests %d/%d.\n", successTests, totalTests);

  return 0;
}


#endif
