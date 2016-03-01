#include <windows.h>
#include <stdio.h>
#include "ini.h"
#include "swnd.h"
#include "config.h"

INI Cfg;
static char CfgFile[_MAX_PATH];

int OptRegex = 0, OptCasesense = 0, OptUtf = 0, OptRecursive = 0;
char *pMask;
char FileMask[MAX_MASK_LEN];
char OptFontFace[LF_FACESIZE+1];
LONG OptFontHeight = 0;
int  OptSaveEncoding = 1;
LONG WindowWidth = 0L;
LONG WindowHeight = 0L;

/******************************************************************************/
void config_init (void)
{
char *tmp;
  strcpy (CfgFile, ConfigPath);
  strcat (CfgFile, "sif2.cfg");
  ini_init (&Cfg, 100);
  ini_load_file (&Cfg, CfgFile);
  //ini_dump (&Cfg, stdout, "");
  ini_create_index (&Cfg);

  OptRegex = ini_search_long (&Cfg, "Settings", "Regex", 0);
  OptCasesense = ini_search_long (&Cfg, "Settings", "CaseSensitive", 0);
  OptUtf = ini_search_long (&Cfg, "Settings", "FilesEncoding", ENC_CP1251);
  OptSaveEncoding = ini_search_long (&Cfg, "Settings", "SaveEncoding", ENC_AUTO);
  OptRecursive = ini_search_long (&Cfg, "Settings", "Recursive", 0);
  pMask = ini_search (&Cfg, "Settings", "FileMask", "*.*");
  strcpy (FileMask, pMask);
  pMask = FileMask;
  OptFontHeight = ini_search_long (&Cfg, "Settings", "FontHeight", -11);
  tmp = ini_search (&Cfg, "Settings", "Font", "Courier New");
  strncpy (OptFontFace, tmp, sizeof(OptFontFace)-1);
  OptFontFace[LF_FACESIZE] = 0;
  WindowWidth = ini_search_long (&Cfg, "Settings", "WindowWidth", DEFAULT_WIDTH);
  WindowHeight = ini_search_long (&Cfg, "Settings", "WindowHeight", DEFAULT_HEIGHT);
  if (WindowWidth < MINIMUM_WIDTH)  {
    WindowWidth = MINIMUM_WIDTH;
  }
  if (WindowHeight < MINIMUM_HEIGHT)  {
    WindowHeight = MINIMUM_HEIGHT;
  }
}

/******************************************************************************/
void config_save (void)
{
FILE *o;
  o = fopen (CfgFile, "w");
  if (o)  {
    fprintf (o, "[Settings]\n");
    fprintf (o, "Regex = %d\n", OptRegex);
    fprintf (o, "CaseSensitive = %d\n", OptCasesense);
    fprintf (o, "FilesEncoding = %d\n", OptUtf);
    fprintf (o, "SaveEncoding = %d\n", OptSaveEncoding);
    fprintf (o, "Recursive = %d\n", OptRecursive);
    fprintf (o, "FileMask = %s\n", FileMask);
    fprintf (o, "FontHeight = %ld\n", OptFontHeight);
    fprintf (o, "Font = %s\n", OptFontFace);
    fprintf (o, "WindowWidth = %ld\n", WindowWidth);
    fprintf (o, "WindowHeight = %ld\n", WindowHeight);
    fclose (o);
  }
}

/******************************************************************************/
void config_destroy (void)
{
  ini_clear (&Cfg);
}