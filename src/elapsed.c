#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "elapsed.h"

/*****************************************************************************
  static char *get_buffer (void)

  Returns pointer to string of GET_BUFFER_SIZE length.

  There are 4 static buffers of such length. Each call this function returns
  next one (on cyclic manner).

  It makes sense in multithreaded environment. Any collision could screw up
  static buffer potentially shared by several threads (there are no special
  means involved to protect it).
  To screw up 4 buffers simultaneously is much harder.

  Parameters:
    -

  Returns:
    Pointer to string.

  Notes:
    internal function.

*****************************************************************************/
#define GET_BUFFER_SIZE   32
static char *get_buffer (void)
{
static char buffer[4][GET_BUFFER_SIZE];
static int  bufindex = 0;
  return buffer[3 & (++bufindex)];
}

/***************************************************************************************/
char *print_big_number (unsigned long val)
{
char *buf = get_buffer();
  if (val < 1000UL)          snprintf (buf, GET_BUFFER_SIZE, "%ld", val);
  else if (val < 1000000UL)  snprintf (buf, GET_BUFFER_SIZE, "%ld %03ld", val/1000, val%1000);
  else snprintf (buf, GET_BUFFER_SIZE, "%ld %03ld %03ld", val/1000000UL, (val/1000)%1000, val%1000);
  return buf;
}

#ifdef _WIN32
/******************************************/
void elapsed_start (ELAPSED *e)
{
  e->StartPoint = GetTickCount();
  e->prevPoint = e->StartPoint;
  e->prevVal = 0;

  e->TmrOK = QueryPerformanceFrequency (&e->TmrFrequency);
  if (e->TmrOK)  {
    e->TmrOK = QueryPerformanceCounter (&e->Counter1);
    e->prevCounter2.QuadPart = e->Counter1.QuadPart;
  }
}
/******************************************/
static void elapsed_stop (ELAPSED *e)
{
  if (e->TmrOK)  {
    e->TmrOK = QueryPerformanceCounter (&e->Counter2);
  }
  e->EndPoint = GetTickCount();
}

/******************************************/
static void create_out (ELAPSED *e)
{
  if (e->TmrOK)  {
    if (e->ddiff < 10000.0)  {
      sprintf (e->buf, "[ %.4f us ]", e->ddiff);
    }
    else  {
      e->ddiff /= 1000.0;
      if (e->ddiff < 10000.0)  {
        sprintf (e->buf, "[ %.4f ms ]", e->ddiff);
      }
      else  {
        e->ddiff /= 1000.0;
        sprintf (e->buf, "[ %.4f s ]", e->ddiff);
      }
    }
  }
  else  {
    sprintf (e->buf, "[ %ld ms ]", e->PointDiff);
  }
}

/******************************************/
char *elapsed_result (ELAPSED *e)
{
  elapsed_stop (e);
  if (e->TmrOK)  {
    e->diff = e->Counter2.QuadPart - e->Counter1.QuadPart;
    e->ddiff = (double)e->diff;
    e->ddiff = 1.0e6*e->ddiff / (double) e->TmrFrequency.QuadPart;
  }
  else  {
    e->PointDiff = e->EndPoint - e->StartPoint;
  }
  create_out (e);
  return e->buf;
}

/******************************************/
char *elapsed_freq (ELAPSED *e)
{
  if (e->TmrOK)  {
    sprintf (e->buf, "HighRes frequency = %I64d Hz.",e->TmrFrequency.QuadPart);
  }
  else  {
    sprintf (e->buf, "No HighRes timers.");
  }
  return e->buf;
}


/******************************************/
static void remaining_out (ELAPSED *e, double percent, unsigned long tmsec)
{
  sprintf (e->buf, "%.2f%% [ %02lu:%02lu:%02lu ]", percent, tmsec / 3600, (tmsec / 60) % 60, tmsec % 60);
}

/******************************************/
char *elapsed_remaining (ELAPSED *e, unsigned long current, unsigned long maxval, const char *suffix)
{
double percent = (double)current * 100.0 / (double)maxval;
double rem;
char bb[64];

unsigned long dval;
double dd;

  elapsed_stop (e);
  dval = current - e->prevVal;

  if (e->TmrOK)  {
    e->diff = e->Counter2.QuadPart - e->Counter1.QuadPart;
    dd = (double)(e->Counter2.QuadPart - e->prevCounter2.QuadPart);
    dd = 1.0e6*dd / (double) e->TmrFrequency.QuadPart;
    dd = (double)dval*1.0e6 / dd;

    rem = (double)e->diff;
    rem = 1.0e6*rem / (double) e->TmrFrequency.QuadPart;
    rem = rem * (100.0 - percent) / percent;
    rem /= 1.0e6;
  }
  else  {
    rem = (double)(e->EndPoint - e->StartPoint);
    dd = (double)(e->EndPoint - e->prevPoint);
    dd = (double)dval*1.0e3 / dd;
    rem = rem * (100.0 - percent) / percent;
    e->PointDiff = (DWORD)rem;
    rem /= 1000.0;
  }
  remaining_out (e, percent, (unsigned long)rem);
  if (dd > 1000)  sprintf (bb, "  %s %s ", print_big_number((unsigned long)dd), suffix ? suffix : "op/s");
  else            sprintf (bb, " %.0f %s ", dd, suffix ? suffix : "op/s");

  strcat (e->buf, bb);

  e->prevVal = current;
  e->prevPoint = e->EndPoint;
  e->prevCounter2.QuadPart = e->Counter2.QuadPart;

  return e->buf;
}














#else

/******************************************/
void elapsed_start (ELAPSED *e)
{
  gettimeofday (&e->StartPoint, NULL);
  memcpy (&e->prevPoint, &e->StartPoint, sizeof(struct timeval));
  e->prevVal = 0;
}
/******************************************/
static void elapsed_stop (ELAPSED *e)
{
  gettimeofday (&e->EndPoint, NULL);
}

/******************************************/
static void create_out (ELAPSED *e, long sec, long usec)
{
  if (sec)  {
    sprintf (e->buf, "[ %ld.%ld s ]", sec, abs(usec) / 1000);
  }
  else  {
    if (usec < 10000)  {
      sprintf (e->buf, "[ %ld us ]", usec);
    }
    else  {
      sprintf (e->buf, "[ %ld.%ld ms ]", usec / 1000, usec % 1000);
    }
  }
}

/******************************************/
char *elapsed_result (ELAPSED *e)
{
  elapsed_stop (e);
  e->PointDiff.tv_sec  = e->EndPoint.tv_sec - e->StartPoint.tv_sec;
  e->PointDiff.tv_usec = e->EndPoint.tv_usec - e->StartPoint.tv_usec;
  create_out (e, e->PointDiff.tv_sec, e->PointDiff.tv_usec);
  return e->buf;
}

/******************************************/
char *elapsed_freq (ELAPSED *e)
{
  sprintf (e->buf, "---");
  return e->buf;
}


/******************************************/
static void remaining_out (ELAPSED *e, double percent, unsigned long tmsec)
{
  sprintf (e->buf, "%.2f%% [ %02lu:%02lu:%02lu ]", percent, tmsec / 3600, (tmsec / 60) % 60, tmsec % 60);
}

/******************************************/
static double diff_us (struct timeval *endt, struct timeval *startt)
{
double res;

  res = (double)(endt->tv_sec - startt->tv_sec)*1.0e6 + (endt->tv_usec - startt->tv_usec);
  return res;
}

/******************************************/
char *elapsed_remaining (ELAPSED *e, unsigned long current, unsigned long maxval, const char *suffix)
{
double percent = (double)current * 100.0 / (double)maxval;
double rem;
char bb[64];

unsigned long dval;
double dd;

  elapsed_stop (e);
  dval = current - e->prevVal;

  rem = diff_us (&e->EndPoint, &e->StartPoint);
  dd  = diff_us (&e->EndPoint, &e->prevPoint);

  dd = (double)dval*1.0e6 / dd;

  rem = rem * (100.0 - percent) / percent;
  rem /= 1.0e6;

  remaining_out (e, percent, (unsigned long)rem);
  if (dd > 1000)  sprintf (bb, "  %s %s ", print_big_number((unsigned long)dd), suffix ? suffix : "op/s");
  else            sprintf (bb, " %.0f %s ", dd, suffix ? suffix : "op/s");
  strcat (e->buf, bb);

  e->prevVal = current;
  memcpy (&e->prevPoint, &e->EndPoint, sizeof(struct timeval));

  return e->buf;
}
#endif
