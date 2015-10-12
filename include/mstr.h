#ifndef MSTR_H_INCLUDED
#define MSTR_H_INCLUDED

#ifdef __cplusplus
  extern "C" {
#endif

char *remove_crlf (char *s);
int  read_line (FILE *in, char *buf, long buflen);
int  m_isspace (int ch);
char *trim_string (char *s);
unsigned long ustrtoul (const char *start, const char *end);
long ustrtol (const char *start, const char *end);
char *replace_chr (char *s, char search, char replace);
char *remove_chr (char *s, char search);
int  tabsep_string (char *s, char **arr, int nCols, char *def);
char *strtoupper (char *dest, const char *src);
char *strtolower (char *dest, const char *src);

void c_escape_string (char *d, const char *s);
void c_unescape_string (char *d);

size_t remove_chr_len (char *s, char search, size_t len);

#ifdef __cplusplus
  }
#endif
#endif