#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "mstr.h"

/************************************************/
char *remove_crlf (char *s)
{
char ch;
char *begin = s;
  while ((ch = *s))  {
    if ((ch == 0x0D) || (ch == 0x0A))  {
      *s = 0;
      break;
    }
    s++;
  }
  return begin;
}

/************************************************/
int read_line (FILE *in, char *buf, long buflen)
{
  while (!feof(in))  {
    if (fgets (buf, buflen, in) != NULL)  {
      //remove_crlf (buf);
      trim_string (buf);
      if ((buf[0] != '#') && (buf[0] != ';'))  {
        return 0;
      }
    }
  }
  return 1;
}

/***********************************/
int m_isspace (int ch)
{
  if (ch <= 32) return 1;
  else          return 0;
}
/*************************************************/
char *trim_string (char *s)
{
char ch;
char *o = s;
char *e;
char *begin = s;

  e = s + strlen(s) - 1;

  if (e < s)  {  /* empty string */
    return begin;
  }

  /* find first non-space */
  while ((ch = *s))  {
    if (!m_isspace(ch)) break;
    s++;
  }

  if (*s == 0)  {  /* no nonspace symbols in string */
    *begin = 0;
    return begin;
  }

  /* find last non-space */
  while (e > s)  {
    if (!m_isspace(*e)) break;
    e--;
  }

  if (begin == s)  {  /* no leading space */
    e++;
    *e = 0;
  }
  else  {

    while (s <= e)  {
      *o++ = *s++;
    }
    *o = 0;
  }
  return begin;
}

/***************************************************************************************/
unsigned long ustrtoul (const char *start, const char *end)
{
const char *s = start;
unsigned long acc;
unsigned char c;

int base = 10;

/*   * See strtol for comments as to the logic used.     */
  do {
    c = *s++;
  } while (isspace(c));
  if (c == '-') {
//    neg = 1;
    c = *s++;
  }
  else if (c == '+') {
    c = *s++;
  }

  if (c == '0')  {
    if (*s == 'x' || *s == 'X') {
      c = s[1];
      s += 2;
      base = 16;
    }
    else base = 8;
  }

  for (acc = 0; ; c = *s++)  {
    if (!isascii(c))
      break;
    if (isdigit(c))  c -= '0';
    else if (isalpha(c))  c -= isupper(c) ? 'A' - 10 : 'a' - 10;
    else break;

    if (c >= base) break;

    acc *= base;
    acc += c;
    if (s == end) break;
  }
  return acc;
}

/***************************************************************************************/
long ustrtol (const char *start, const char *end)
{
const char *s = start;
long acc;
unsigned char c;
int neg = 0;
int base = 10;

/*   * See strtol for comments as to the logic used.     */
  do {
    c = *s++;
  } while (isspace(c));
  if (c == '-') {
    neg = 1;
    c = *s++;
  }
  else if (c == '+') {
    c = *s++;
  }

  if (c == '0')  {
    if (*s == 'x' || *s == 'X') {
      c = s[1];
      s += 2;
      base = 16;
    }
    else base = 8;
  }

  for (acc = 0; ; c = *s++)  {
    if (!isascii(c))
      break;
    if (isdigit(c))  c -= '0';
    else if (isalpha(c))  c -= isupper(c) ? 'A' - 10 : 'a' - 10;
    else break;

    if (c >= base) break;

    acc *= base;
    acc += c;
    if (s == end) break;
  }
  return neg ? -acc : acc;
}

/*************************************************/
char *replace_chr (char *s, char search, char replace)
{
char ch;
char *begin = s;

  while ((ch = *s))  {
    if (ch == search) *s = replace;
    else *s = ch;
    s++;
  }
  return begin;
}

/*************************************************/
char *remove_chr (char *s, char search)
{
char ch;
char *begin = s;
char *d = s;

  while ((ch = *s))  {
    if (ch != search)  {
      *d++ = ch;
    }
    s++;
  }
  *d = 0;
  return begin;
}

/*************************************************/
size_t remove_chr_len (char *s, char search, size_t len)
{
char ch;
char *begin = s;
char *d = s;
size_t i;

  for (i = 0; i < len; i++)  {
    ch = *s++;
    if (ch != search)  {
      *d++ = ch;
    }
  }
  return (d - begin);
}

/*****************************************************************/
int tabsep_string (char *s, char **arr, int nCols, char *def)
{
char ch;
char *p = s;
int result, cnt = 0;

  while ((ch = *s))  {
    if (ch == '\t')  {
      *s = 0;
      if (cnt < nCols)  {
        arr[cnt++] = p;
      }
      p = s + 1;
    }
    s++;
  }
  if (cnt < nCols) arr[cnt++] = p;
  result = cnt;
  for (; cnt < nCols; cnt++)  {
    arr[cnt] = def;
  }
  return result;
}
/*****************************************************************/
char *strtoupper (char *dest, const char *src)
{
char ch;
char *begin = dest;
  while ((ch = *src))  {
    *dest++ = toupper (ch);
    src++;
  }
  *dest = 0;
  return begin;
}

/*****************************************************************/
char *strtolower (char *dest, const char *src)
{
char ch;
char *begin = dest;
  while ((ch = *src))  {
    *dest++ = tolower (ch);
    src++;
  }
  *dest = 0;
  return begin;
}

/*******************************************/
void c_escape_string (char *d, const char *s)
{
char ch;
  while ((ch = (*s++)))  {
    if (ch == '\\')  {
      *d++ = '\\';
      *d++ = '\\';
    }
    else if (ch == 0x0D)  {
      *d++ = '\\';
      *d++ = 'r';
    }
    else if (ch == 0x0A)  {
      *d++ = '\\';
      *d++ = 'n';
    }
    else if (ch == 9)  {
      *d++ = '\\';
      *d++ = 't';
    }
    else  {
      *d++ = ch;
    }
  }
  *d = 0;
}

/*******************************************/
void c_unescape_string (char *d)
{
char *s = d;
char ch;
  while ((ch = (*s++)))  {
    if (ch == '\\')  {
      ch = (*s++);
      if (ch == 0)  break;
      else if (ch == '\\') *d++ = '\\';
      else if (ch == 'r')  *d++ = 0x0D;
      else if (ch == 'n')  *d++ = 0x0A;
      else if (ch == 't')  *d++ = ' ';/*9;*/
      else                 *d++ = ch;
    }
    else  {
      *d++ = ch;
    }
  }
  *d = 0;
}
