#ifndef __SWND_H__
#define __SWND_H__

// identifiers.
#define IDC_LFIND     2000
#define IDC_RECENT    2001
#define IDC_RTF       2002
#define IDC_STATUS0   2003
#define IDC_SEARCH    2004
#define IDC_GROUP0    2005
#define IDC_RECURSIV  2006
#define IDC_ENCODING  2007
#define IDC_SAVECRLF  2008
#define IDC_STATIC1   2009
#define IDC_STATIC2   2010
#define IDC_MASK      2011
#define IDC_SAVELF    2012
#define IDC_LISTVIEW0 2013
#define IDC_UPD       2014
#define IDC_DIRS      2015
#define IDC_FILES     2016
#define IDC_MANPATH   2017
#define IDC_REALPATH  2018
#define IDC_CASE      2019
#define IDC_TXT       2020
#define IDC_STATIC0   2021
#define IDC_DRIVES    2022
#define IDC_RE        2023
#define IDC_FONT      2024
#define IDC_SENCODING 2025


extern HWND  hMainWindow;
extern HWND  hResult;
extern HWND  hRTF;

extern LOGFONT clfnt;

extern char MyPath[_MAX_PATH];
extern char CurrentPath[_MAX_PATH];
extern char ConfigPath[_MAX_PATH];

void get_options (void);

void DoEvents (HWND hWnd);
void do_events (void);
void status_bar (int part, const char *fmt, ...);
void msgbox (const char *fmt, ...);
BOOL DirectoryExists(LPCTSTR szPath);

#endif
