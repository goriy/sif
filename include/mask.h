#ifndef MASK_H_INCLUDED
#define MASK_H_INCLUDED
#ifdef __cplusplus
  extern "C" {
#endif

typedef struct mask_s  {
  int cnt;
  int allocated;
  char bulk[MAX_MASK_LEN];
  char **tokens;
} mask_t;

int mask_match (const char *expr, const char *mask);

int  masks_prepare (const char *mask, mask_t *m);
void masks_unprepare (mask_t *m);

int masks_match (const char *expr, mask_t *mask);

#ifdef __cplusplus
  }
#endif
#endif
