#ifndef ELAPSED_H_INCLUDED
#define ELAPSED_H_INCLUDED

#ifdef _WIN32

#include <windows.h>

typedef struct ELAPSED  {
  DWORD StartPoint;
  DWORD EndPoint;
  DWORD PointDiff;

  LARGE_INTEGER TmrFrequency;
  LARGE_INTEGER Counter1;
  LARGE_INTEGER Counter2;

  BOOL TmrOK;
  LONGLONG diff;
  double ddiff;

  char buf[100];

  LARGE_INTEGER prevCounter2;
  long          prevVal;
  DWORD         prevPoint;

} ELAPSED;

#else

#include <sys/time.h>

typedef struct ELAPSED  {
  struct timeval StartPoint;
  struct timeval EndPoint;
  struct timeval PointDiff;

  double ddiff;

  char buf[100];

  unsigned long  prevVal;
  struct timeval prevPoint;

} ELAPSED;


#endif

char *print_big_number (unsigned long val);

void elapsed_start (ELAPSED *e);
char *elapsed_result (ELAPSED *e);
char *elapsed_freq (ELAPSED *e);
char *elapsed_remaining (ELAPSED *e, unsigned long current, unsigned long maxval, const char *suffix);

#endif
