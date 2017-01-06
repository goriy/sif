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
#include <wchar.h>
#include "elapsed.h"

static int FoundEntries = 0;
static const char *initial_path = NULL;
static int initial_path_len = 0;

char    fbuf[FBUF_SIZE];
size_t  loaded_len = 0;

wchar_t fbufUnicode[FBUF_SIZE];
int     unicode_len = 0;

wchar_t fbufForSearch[FBUF_SIZE];

static wchar_t *BufferToSearch;

static wchar_t LookFor[LINE_LEN];
static int     LookForLen = 0;
static char *LookFor1251;
static int   LookFor1251Len = 0;
static char  LookForUTF8[LINE_LEN*4];
static int   LookForUTF8Len = 0;

char loaded_file[_MAX_PATH] = {0};
int  loaded_flags = 0;

char shown_file[_MAX_PATH] = {0};

Q_DEFINITION_MACRO(AllFiles);

typedef struct found_file_t  {
  que_t  elem;
  size_t basepath_len;
  char   name[_MAX_PATH];
  unsigned char flags;
  que_t  results;

} found_file_t;

typedef struct found_result_t  {
  que_t    elem;
//  struct found_file_t *file;

  size_t   line;
  size_t   linebegin;
  size_t   lineend;
  size_t   column;
  size_t   offset;
  size_t   len;
//  char     text[LINE_LEN];
} found_result_t;


kmem_cache_t FileCache = KMEM_CACHE_INITIALIZER(sizeof(found_file_t),200);
kmem_cache_t ResCache  = KMEM_CACHE_INITIALIZER(sizeof(found_result_t),300);

found_file_t *CurrentFile = NULL;

/*
static void result_add (char *filename, int row, int col, unsigned long offset,
                 const char *line, int flags, char *fullname, int sellen,
                 unsigned long linebegin , unsigned long lineend);
*/
static void result_add (found_file_t *file, found_result_t *result, const wchar_t *text, int text_len);

static ELAPSED Elapsed;

/******************************************************************************/
static void que_delete_all (que_t *q, kmem_cache_t *cache)
{
que_t *data;
  while ((data = q_get (q)) != q)  {
    kmem_cache_free (cache, data);
  }
}

/******************************************************************************/
void search_init (void)
{
  kmem_cache_prealloc (&FileCache, 200);
  kmem_cache_prealloc (&ResCache, 300);
}

/******************************************************************************/
void search_clear (void)
{
found_file_t *ff;
  Q_WALK_MACRO (&AllFiles, ff)  {
    que_delete_all (&ff->results, &ResCache);
  }
  que_delete_all (&AllFiles, &FileCache);

}

/******************************************************************************/
void search_destroy (void)
{
  search_clear ();
  kmem_cache_purge (&FileCache);
  kmem_cache_purge (&ResCache);
}

/******************************************************************************/
static void makeupper (wchar_t *buf, size_t len, wchar_t *dest)
{
  if (buf != dest)  {
    memcpy (dest, buf, len * sizeof(wchar_t));
  }
  CharUpperBuffW (dest, len);
}

/******************************************************************************/
static found_file_t *find_file_in_results (const char *filename)
{
found_file_t *ff;
  Q_WALK_MACRO (&AllFiles, ff)  {
    if (!strcmp(filename, ff->name))  return ff;
  }
  return NULL;
}

/******************************************************************************/
static found_result_t *find_result_in_file (found_file_t *ff, size_t offset)
{
found_result_t *fr;
  Q_WALK_MACRO (&ff->results, fr)  {
    if (fr->offset == offset)  return fr;
  }
  return NULL;
}

/******************************************************************************/
static int is_file_binary (char *dat, size_t len)
{
size_t i;
char ch;
  for (i = 0; i < len; i++)  {
    ch = *dat++;
    if (ch < 9)  return 1;
    if ((ch > 0x0D) && (ch < 32))  return 1;
  }
  return 0;
}

/******************************************************************************/
static int load_file_to_fbuf (const char *fullname, int for_search)
{
FILE *in;
int ret = 0;
size_t new_loaded_len;
int tmp;

  if (!for_search)  {
    // do not reload already loaded file only if it's for show, not for search.
    // Searching should not use cached data, because loaded file content is
    // suitable for show, not for text search (due to missing case conversion,
    // for example)
    if (!strcmp(loaded_file, fullname))  {
      return 1;  // do not load if already loaded
    }
  }

  in = fopen (fullname, "rb");
  if (!in)  {
    //printf ("unable to open file (Error %ld, errno=%d)\n", GetLastError(), errno);
    return ret;
  }

  loaded_flags = 0;

  if (feof(in))  {  // if empty file
    fclose (in);
    return ret;
  }

  loaded_len = fread (fbuf, sizeof(unsigned char), FBUF_SIZE-1, in);
  fbuf[loaded_len] = 0;
  fbuf[FBUF_SIZE-1] = 0;
  fclose (in);

  if (is_file_binary(fbuf, loaded_len))  {
    loaded_flags = FLAG_BIN;
    return 1;
  }

  new_loaded_len = remove_chr_len (fbuf, 0x0D, loaded_len);
  fbuf[new_loaded_len] = 0;
  if (new_loaded_len != loaded_len)  {
    loaded_flags |= FLAG_CRLF;
    loaded_len = new_loaded_len;
  }

  if (OptUtf == ENC_AUTO)  {
    SetLastError (0);
    tmp = MultiByteToWideChar (CP_UTF8, MB_ERR_INVALID_CHARS, fbuf, loaded_len, fbufUnicode, FBUF_SIZE);
    fbufUnicode[tmp] = 0;
    if (GetLastError() == 0)  {
      loaded_flags |= FLAG_UTF8;
    }
    else  {
      //printf ("Last Error = %lu\n", GetLastError());
      tmp = MultiByteToWideChar (CP_ACP, MB_ERR_INVALID_CHARS, fbuf, loaded_len, fbufUnicode, FBUF_SIZE);
      fbufUnicode[tmp] = 0;
    }
  }
  else if (OptUtf == ENC_CP1251) {
    tmp = MultiByteToWideChar (CP_ACP, MB_ERR_INVALID_CHARS, fbuf, loaded_len, fbufUnicode, FBUF_SIZE);
    fbufUnicode[tmp] = 0;
  }
  else  {
    tmp = MultiByteToWideChar (CP_UTF8, 0, fbuf, loaded_len, fbufUnicode, FBUF_SIZE);
    fbufUnicode[tmp] = 0;
    loaded_flags |= FLAG_UTF8;
  }
  unicode_len = tmp;

  if ((for_search) && (!OptCasesense))  {
    makeupper (fbufUnicode, unicode_len, fbufForSearch);
    BufferToSearch = fbufForSearch;
  }
  else  {
    BufferToSearch = fbufUnicode;
  }

  //printf ("%s: fread %u bytes\n", fullname + initial_path_len, loaded_len);
  if (!for_search)  {
    // remember loaded filename only if it was loaded not for search,
    // because search process corrupts buffer content
    strcpy (loaded_file, fullname);
  }
  ret = 1;
  return ret;
}

/******************************************************************************/
int find_in_file (char *fullname)
{
size_t l, i;
wchar_t *str = NULL;
wchar_t *p, *cp;
wchar_t c;
int line, column, offset = 0;
int linelen = 0;

  l = unicode_len;

  if (CurrentFile == NULL)  {
    CurrentFile = kmem_cache_alloc (&FileCache);
    if (!CurrentFile)  {
      return 0;
    }
  }
  strcpy (CurrentFile->name, fullname);
  CurrentFile->basepath_len = initial_path_len;
  CurrentFile->flags = loaded_flags;
  Q_NEW (&CurrentFile->results);

  str = BufferToSearch;
  p = str;
  line = 1;

  for (i = 0; i < l; i++)  {
    c = *p;
    if ((c == 0x0A) || (i == (l-1)))  {
      wchar_t *fnd;
      //printf ("%s: found lf at 0x%X (str = %d)\n", fullname + initial_path_len, p - fbuf, str - fbuf);

      *p = 0;
      cp = str;

      while ((fnd = wcsstr (cp, LookFor)))  {
        found_result_t *res;

        column = fnd - str + 1;
        //offset = fnd - fbuf;

        res = kmem_cache_alloc (&ResCache);
        if (res)  {
          int stringlen;
          //res->file = CurrentFile;
          res->line = line;
          res->linebegin = offset;
          res->lineend = offset + linelen;
          res->column = column;
          res->offset = offset + (fnd - str);
          res->len = LookFor1251Len;
          //strncpy(res->text, str, LINE_LEN-1);
          //res->text[LINE_LEN-1] = 0;

          q_put (&CurrentFile->results, res);

          fbufUnicode[p - BufferToSearch] = 0;
          stringlen = p - str;
          result_add (CurrentFile, res, fbufUnicode + (str - BufferToSearch), stringlen >= LINE_LEN ? (LINE_LEN-1) : stringlen);
          FoundEntries++;
        }
        cp = fnd + 1;
      }
      str = p + 1;
      line++;
      offset += (linelen + 1);
      linelen = 0;
    }
    else  {
      linelen++;
    }
    p++;
  }
  if (!Q_EMPTY(&CurrentFile->results))  {
    q_put (&AllFiles, CurrentFile);
    CurrentFile = NULL;
  }
  return 1;
}

/******************************************************************************/
int find_in_binary_file (char *fullname)
{
size_t i;
char *src;
int srclen;
char *fb = fbuf;


  if (OptUtf == ENC_UTF8)  {
    src = LookForUTF8;
    srclen = LookForUTF8Len;
  }
  else  {
    src = LookFor1251;
    srclen = LookFor1251Len;
  }

  if (CurrentFile == NULL)  {
    CurrentFile = kmem_cache_alloc (&FileCache);
    if (!CurrentFile)  {
      return 0;
    }
  }
  strcpy (CurrentFile->name, fullname);
  CurrentFile->basepath_len = initial_path_len;
  CurrentFile->flags = loaded_flags;
  Q_NEW (&CurrentFile->results);

  for (i = 0; i < loaded_len; i++)  {
    if (*fb == src[0])  {
      if (memcmp (fb, src, srclen)) {
        fb++;
      }
      else  {
        result_add (CurrentFile, NULL, NULL, i);
        q_put (&AllFiles, CurrentFile);
        CurrentFile = NULL;
        return 1;
      }
    }
    else  {
      fb++;
    }
  }
  return 1;
}

/******************************************************************************/
int fcallback(const char *path, char *name, struct _finddata_t *f, int depth)
{
char *npath;
size_t tmp;

  tmp = strlen(path);
  npath = (char *)malloc(tmp+5+strlen(name));
  if (!npath) return 1;
  strcpy (npath, path);
  if ((npath[tmp-1] != '\\') && (npath[tmp-1] != '/'))  {
    strcat (npath, "/");
  }
  strcat (npath, name);
  status_bar (1, "%s", npath + initial_path_len);
  do_events ();
  if (load_file_to_fbuf (npath, 1))  {
    if (loaded_flags & FLAG_BIN)  {
      find_in_binary_file (npath);
    }
    else  {
      find_in_file (npath);
    }
  }
  free (npath);
  return 1;
}

/******************************************************************************/
static int dcallback(const char *path, char *name, struct _finddata_t *f, int depth)
{
  status_bar (1, "%s", path + initial_path_len);
  do_events ();
  return 1;
}

/******************************************************************************/
void search_in_path (char *lookfor, const char *path, const char *mask)
{
static int search_in_progress = 0;

  if (search_in_progress)  return;

  search_in_progress = 1;
  elapsed_start (&Elapsed);
  status_bar (0, "");
  status_bar (1, "");
  initial_path = path;
  initial_path_len = strlen(path);
  FoundEntries = 0;
  LookFor1251 = lookfor;
  LookFor1251Len = strlen(LookFor1251);
  LookForLen = MultiByteToWideChar (CP_ACP, MB_ERR_INVALID_CHARS, lookfor, -1, LookFor, LINE_LEN);
  LookForUTF8Len = WideCharToMultiByte (CP_UTF8, 0, LookFor, LookForLen, LookForUTF8, sizeof(LookForUTF8)-1, NULL, NULL);
  LookForUTF8[LookForUTF8Len] = 0;

  if (!OptCasesense)  {
    makeupper (LookFor, LookForLen, LookFor);
  }

  /* delete all previous search results */
  search_clear ();

  /* Drop name of currently loaded file. Force to re-show it later. */
  shown_file[0] = 0;

  /* Drop name of currently loaded file. Force to reload on next load call.
   * It's necessary because currently loaded file content is suitable for
   * show, not for text search (missing case conversion, for example). */
  loaded_file[0] = 0;

  dirwalk (path, mask, OptRecursive, fcallback, dcallback);

  if (CurrentFile != NULL)  {
    kmem_cache_free (&FileCache, CurrentFile);
    CurrentFile = NULL;
  }

  #if WITH_ALLOC_COUNT
  status_bar (1, "Found: %d %s (%ld %ld)", FoundEntries, elapsed_result(&Elapsed), FileCache.allocated, ResCache.allocated);
  #else
  status_bar (1, "Found: %d %s", FoundEntries, elapsed_result(&Elapsed));
  #endif
  search_in_progress = 0;
}

/******************************************************************************/
static int show_and_highlight_file (found_file_t *ff)
{
CHARFORMAT2 cfline, cfresult, cfall;
size_t prev_line = 0;
int ret = 1;
found_result_t *fr;

  if (!load_file_to_fbuf (ff->name, 0))  return ret;


  if (ff->flags & FLAG_BIN)  {
    Edit_SetText (hRTF, "<<binary file>>");
    Button_Enable (GetDlgItem (hMainWindow, IDC_SAVELF), 0);
    Button_Enable (GetDlgItem (hMainWindow, IDC_SAVECRLF), 0);
    Edit_SetText (GetDlgItem(hMainWindow, IDC_LFIND), "BIN");
    shown_file[0] = 0;
    return ret;
  }

  if ((ff->flags & FLAG_CRLF) == 0)  {
    if (ff->flags & FLAG_UTF8)  {
      Edit_SetText (GetDlgItem(hMainWindow, IDC_LFIND), "LF UTF-8");
    }
    else  {
      Edit_SetText (GetDlgItem(hMainWindow, IDC_LFIND), "LF 1251");
    }
  }
  else  {
    if (ff->flags & FLAG_UTF8)  {
      Edit_SetText (GetDlgItem(hMainWindow, IDC_LFIND), "CRLF UTF-8");
    }
    else  {
      Edit_SetText (GetDlgItem(hMainWindow, IDC_LFIND), "CRLF 1251");
    }
  }

  if (!strcmp(shown_file, ff->name))  {
    ret = 0;
  }
  else  {
    strcpy (shown_file, ff->name);

    SendMessage(hRTF, WM_SETREDRAW, 0, 0);

    //int i = WideCharToMultiByte (CP_ACP, WC_NO_BEST_FIT_CHARS, fbufUnicode, unicode_len, fbuf, sizeof(fbuf)-1, "?", NULL);
    //fbuf[i] = 0;

    //Edit_SetTextW (hRTF, fbuf);
    SetWindowTextW (hRTF, fbufUnicode);

    Button_Enable (GetDlgItem (hMainWindow, IDC_SAVELF), 1);
    Button_Enable (GetDlgItem (hMainWindow, IDC_SAVECRLF), 1);

    SendMessage(hRTF, EM_HIDESELECTION, 1, 0);

    memset (&cfall, 0, sizeof(cfall));
    cfall.cbSize = sizeof(cfall);
    cfall.dwMask = CFM_FACE;
    cfall.dwEffects = CFE_AUTOBACKCOLOR | CFE_AUTOCOLOR;
    strcpy(cfall.szFaceName, OptFontFace);

    Edit_SetSel (hRTF, 0, -1);
    SendMessage(hRTF, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM) &cfall);


    memset (&cfline, 0, sizeof(cfline));
    cfline.cbSize = sizeof(cfline);
    cfline.dwMask = CFM_COLOR;
    cfline.crTextColor = RGB(0, 0, 255);
    cfline.dwEffects = 0;

    memset (&cfresult, 0, sizeof(cfresult));
    cfresult.cbSize = sizeof(cfresult);
    cfresult.dwMask = CFM_COLOR | CFM_BOLD | CFM_BACKCOLOR;
    cfresult.crTextColor = RGB(255, 0, 0);
    cfresult.crBackColor = RGB(190, 255, 190);
    cfresult.dwEffects = 0; //CFE_BOLD;

    Q_WALK_MACRO (&ff->results, fr)  {
      if (prev_line != fr->line)  {
        prev_line = fr->line;
        Edit_SetSel (hRTF, fr->linebegin, fr->lineend);
        SendMessage(hRTF, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM) &cfline);
      }
      Edit_SetSel (hRTF, fr->offset, fr->offset + fr->len);
      SendMessage(hRTF, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM) &cfresult);
    }

    SendMessage(hRTF, EM_HIDESELECTION, 0, 0);
    ret = 0;
    SendMessage(hRTF, WM_SETREDRAW, 1, 0);
    RedrawWindow(hRTF, NULL, NULL, RDW_ERASE | RDW_FRAME | RDW_INVALIDATE | RDW_ALLCHILDREN);
  }
  return ret;
}

/******************************************************************************/
int show_and_highlight_file_ex (const char *fullname)
{
found_file_t *ff;
int ret;

  status_bar (0, "%s", fullname);

  if (!load_file_to_fbuf (fullname, 0))  return 1;

  if (!strcmp(shown_file, fullname))  {
    return 0;
  }

  ff = find_file_in_results (fullname);
  if (!ff) {
    ff = kmem_cache_alloc (&FileCache);
    if (!ff)  {
      return 0;
    }
    strcpy (ff->name, fullname);
    ff->basepath_len = 0;
    ff->flags = loaded_flags;
    Q_NEW (&ff->results);
    q_put (&AllFiles, ff);
  }
  ret = show_and_highlight_file (ff);
  Edit_SetSel (hRTF, 0, 0);
  Edit_ScrollCaret (hRTF);
  return ret;
}

/******************************************************************************/
static void move_scroll_to_result (found_result_t *fr)
{
    Edit_SetSel (hRTF, 0, 0);
    Edit_ScrollCaret (hRTF);

    unsigned long Pos = SendMessage(hRTF, EM_POSFROMCHAR, fr->offset, 0);
    //int x = LOWORD(Pos);
    int y = HIWORD(Pos);
    //printf ("x=%d. y=%d\n", x, y);

    RECT rect;
    GetClientRect(hRTF, &rect);
    int h = (rect.bottom - rect.top)/2;
    if (y > h)  {
      SendMessage (hRTF, WM_VSCROLL, MAKEWPARAM(SB_THUMBPOSITION,y - h), 0);
    }
    Edit_SetSel (hRTF, fr->linebegin, fr->linebegin);
}

/******************************************************************************/
void on_results_click (int item)
{
char offsetT[32], fln[_MAX_PATH];
unsigned long offset;
found_file_t *ff;
found_result_t *fr;

  ListView_GetItemText(hResult, item, 3, fln, sizeof(fln));
  ListView_GetItemText(hResult, item, 2, offsetT, sizeof(offsetT));
  offset = strtoul (offsetT, NULL, 10);
  status_bar (0, "%s", fln);
  //printf ("fln = %s, offsetT=%s\n", fln, offsetT);

  ff = find_file_in_results (fln);
  if (!ff) {
    return;
  }

  show_and_highlight_file (ff);
  fr = find_result_in_file (ff, offset);
  if (fr)  move_scroll_to_result (fr);
}

/******************************************************************************/
static void result_add (found_file_t *file, found_result_t *result, const wchar_t *text, int text_len)
{
LV_ITEM item;
HWND wnd = hResult;
int index;
int i;
char buf[_MAX_PATH];
static char tbuf[LINE_LEN*2];

  memset (&item, 0, sizeof (LV_ITEM));

  item.mask = LVIF_TEXT;
  item.iSubItem = 0;
  item.iItem = 100000;
  item.pszText = "";
  index = ListView_InsertItem(wnd, &item);

  strcpy (buf, file->name + file->basepath_len);
  i = strlen (buf);
  buf[i++] = ' ';
  while (i & 3)  {
    buf[i++] = ' ';
  }
  buf[i] = 0;

  if (file->flags & FLAG_BIN)  {
    snprintf (buf + i, sizeof(buf) - i, "(bin: offset 0x%x):", text_len);
    ListView_SetItemText (wnd, index, 0, buf);
    ListView_SetItemText (wnd, index, 1, "");
    snprintf (buf, sizeof(buf), "%u", text_len);
    ListView_SetItemText (wnd, index, 2, buf);
    ListView_SetItemText (wnd, index, 3, file->name);
  }
  else  {
    snprintf (buf + i, sizeof(buf) - i, "(line %d, col %d):", result->line, result->column);
    ListView_SetItemText (wnd, index, 0, buf);

    i = WideCharToMultiByte (CP_ACP, WC_NO_BEST_FIT_CHARS, text, text_len, tbuf, sizeof(tbuf)-1, "?", NULL);
    tbuf[i] = 0;
    ListView_SetItemText (wnd, index, 1, tbuf);

    snprintf (buf, sizeof(buf), "%u", result->offset);
    ListView_SetItemText (wnd, index, 2, buf);
    ListView_SetItemText (wnd, index, 3, file->name);
  }
  do_events ();
}
