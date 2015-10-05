#ifndef DIRWALK_H_INCLUDED
#define DIRWALK_H_INCLUDED

#define DIR_SEPARATOR   "/"

typedef int (*DWALK_FUNC)(const char *path, char *name, struct _finddata_t *f, int depth);

extern void dirwalk (const char *path, const char *filter, int recurse, DWALK_FUNC ffunc, DWALK_FUNC dfunc);
extern void path_mask (char *full, char **path, char **mask);

#endif
