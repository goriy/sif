#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "config.h"
#include <commctrl.h>        // INCLUDE LIBRARY comctl32.lib INTO YOUR PROJECT!
#include <Windowsx.h>
#include <richedit.h>
#include "swnd.h"
#include <io.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "dirwalk.h"
#include "search.h"
#include "que.h"
#include "kmem_cache.h"
#include <ctype.h>
#include "mstr.h"

static char BakFile[_MAX_PATH];

/******************************************************************************/
void on_save_click (int crlf)
{
wchar_t *rtftext;
int i, rtflen;
FILE *o;
char ch, *p;
char *utfstring;
int utflen;

/*
  char dst[16];
  int dstlen = WideCharToMultiByte (CP_UTF8, 0, L"123", 3, dst, 16, NULL, NULL);
  dst[dstlen] = 0;
  printf ("len=%d, s='%s', err=%ld\n", dstlen, dst, GetLastError());
return;
*/

  get_options();
  rtflen = GetWindowTextLengthW(hRTF);
//  printf ("GetWindowTextLengthW = %d\n", rtflen);
  rtftext = malloc ((rtflen + 1) * sizeof(wchar_t));
  if (!rtftext)  {
    msgbox ("Недостаточно памяти. Файл не сохранен.");
    return;
  }

  if ((OptSaveEncoding == ENC_UTF8) && (!(loaded_flags & FLAG_UTF8)))  {
    if (MessageBox(hMainWindow, "Файл был в кодировке CP1251, а запрос на сохранение в UTF-8. Все равно сохранить?",
                                "Кодировка", MB_YESNO|MB_ICONWARNING|MB_DEFBUTTON2) == IDNO)  {
      free (rtftext);
      return;
    }
  }
  else if ((OptSaveEncoding == ENC_CP1251) && (loaded_flags & FLAG_UTF8))  {
    if (MessageBox(hMainWindow, "Файл был в кодировке UTF-8, а запрос на сохранение в CP1251. Все равно сохранить?",
                                "Кодировка", MB_YESNO|MB_ICONWARNING|MB_DEFBUTTON2) == IDNO)  {
      free (rtftext);
      return;
    }
  }
  else if (OptSaveEncoding == ENC_AUTO)  {
    if (loaded_flags & FLAG_UTF8)  OptSaveEncoding = ENC_UTF8;
    else                           OptSaveEncoding = ENC_CP1251;
  }

  rtflen = GetWindowTextW (hRTF, rtftext, rtflen);
  rtftext[rtflen] = 0;
//  printf ("wcslen = %d (ret = %d)\n", wcslen (rtftext), rtflen);

  if (OptSaveEncoding == ENC_UTF8)  {
    utfstring = malloc (rtflen * 4 + 1);
    if (!utfstring)  {
      free (rtftext);
      msgbox ("Недостаточно памяти. Файл не сохранен.");
      return;
    }
    utflen = WideCharToMultiByte (CP_UTF8, 0, rtftext, rtflen, utfstring, rtflen * 4, NULL, NULL);
    utfstring[utflen] = 0;
  }
  else  {
    utfstring = malloc (rtflen * 2 + 1);
    if (!utfstring)  {
      free (rtftext);
      msgbox ("Недостаточно памяти. Файл не сохранен.");
      return;
    }
    utflen = WideCharToMultiByte (CP_ACP, WC_NO_BEST_FIT_CHARS, rtftext, rtflen, utfstring, rtflen * 2, "?", NULL);
    utfstring[utflen] = 0;
  }
  free (rtftext);

  strcpy (BakFile, loaded_file);
  strcat (BakFile, ".bak");
  if (CopyFile (loaded_file, BakFile, FALSE) == 0)  {
    free (utfstring);
    msgbox ("Невозможно скопировать файл .bak. Файл не сохранен.");
    return;
  }

  o = fopen (loaded_file, "wb");
  if (!o)  {
    free (utfstring);
    msgbox ("Невозможно открыть файл на запись. Файл не сохранен.");
    return;
  }

  p = utfstring;
  for (i = 0; i < rtflen; i++)  {
    ch = *p++;
    if (ch == 0x0A)  {
      if (crlf)  fputc (0x0D, o);
      fputc (0x0A, o);
    }
    else if (ch != 0x0D)  {
      fputc (ch, o);
    }
  }
  fclose (o);
  free (rtftext);
}
