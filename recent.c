
#include <windows.h>
#include <Windowsx.h>
#include <stdio.h>
#include <string.h>
#include "swnd.h"
#include "mstr.h"

#define RECENT_COUNT  40
#define RECENT_SIZE   1024

static char Recent[RECENT_COUNT][RECENT_SIZE] = {{0}};
static int RecentCount = 0;

static char RecentFile[_MAX_PATH];
static int RecentCurrent = 0;

/******************************************************************************/
static void recent_save (void)
{
FILE *o;
int i;
  strcpy (RecentFile, ConfigPath);
  strcat (RecentFile, "recent.txt");
  o = fopen (RecentFile, "w");
  if (o)  {
    for (i = 0; i < RecentCount; i++)  {
      fprintf (o, "%s\n", Recent[i]);
    }
    fclose (o);
  }
}

/******************************************************************************/
void recent_load (void)
{
FILE *o;
int i;
  strcpy (RecentFile, ConfigPath);
  strcat (RecentFile, "recent.txt");
  o = fopen (RecentFile, "r");
  if (o)  {
    RecentCount = 0;
    for (i = 0; i < RECENT_COUNT; i++)  {
      if (feof(o)) break;
      fgets (Recent[i], RECENT_SIZE-1, o);
      Recent[i][RECENT_SIZE-1] = 0;
      remove_crlf (Recent[i]);
      if (strlen(Recent[i]) == 0)  {
        i--;
        continue;
      }
      RecentCount++;
    }
    fclose (o);
  }
}

/******************************************************************************/
static void recent_real_add (const char *text)
{
int i;
  for (i = RECENT_COUNT - 1; i > 0; i--)  {
    strncpy (Recent[i], Recent[i-1], RECENT_SIZE);
  }
  strncpy (Recent[0], text, RECENT_SIZE - 1);
  Recent[0][RECENT_SIZE-1] = 0;
  RecentCount++;
  if (RecentCount > RECENT_COUNT)  {
    RecentCount = RECENT_COUNT;
  }
}

/******************************************************************************/
void recent_add (const char *text)
{
int i;
  for (i = 0; i < RecentCount; i++)  {
    if (!strcmp (Recent[i], text))  {
      break;
    }
  }
  if (i >= RecentCount)  {
    recent_real_add (text);
  }
  else  {
    for (; i > 0; i--)  {
      strncpy (Recent[i], Recent[i-1], RECENT_SIZE);
    }
    strncpy (Recent[0], text, RECENT_SIZE - 1);
    Recent[0][RECENT_SIZE-1] = 0;
  }
  recent_save ();
}

/******************************************************************************/
void recent_fill_combo (int id)
{
HWND wnd;
int i;
  wnd = GetDlgItem(hMainWindow, id);
  ComboBox_ResetContent (wnd);
  for (i = 0; i < RecentCount; i++)  {
    ComboBox_AddString (wnd, Recent[i]);
  }
}

/******************************************************************************/
const char *recent_get_by_list (int id)
{
HWND wnd;
int index;

  wnd = GetDlgItem(hMainWindow, id);
  index = ComboBox_GetCurSel (wnd);
  if ((index >= 0) && (index < RECENT_COUNT-1))  {
    ComboBox_SetCurSel (wnd, -1);
    return Recent[index];
  }
  return "";
}

/******************************************************************************/
const char *recent_current_increment (void)
{
const char *ret;
  if ((RecentCurrent >= 0) && (RecentCurrent < RecentCount))  {
    ret = Recent[RecentCurrent];
  }
  else  {
    ret = "";
  }
  RecentCurrent++;
  if (RecentCurrent >= RecentCount)  RecentCurrent = RecentCount - 1;
  return ret;
}

/******************************************************************************/
const char *recent_current_decrement (void)
{
const char *ret;
  if ((RecentCurrent >= 0) && (RecentCurrent < RecentCount))  {
    ret = Recent[RecentCurrent];
  }
  else  {
    ret = "";
  }
  RecentCurrent--;
  if (RecentCurrent < 0)  RecentCurrent = 0;
  return ret;
}

/******************************************************************************/
void recent_current_reset (void)
{
  RecentCurrent = 0;
}
