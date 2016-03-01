#ifndef __CONFIG_H__
#define __CONFIG_H__

#define ENC_AUTO    0
#define ENC_CP1251  1
#define ENC_UTF8    2

#define MAX_MASK_LEN   1024

extern int OptRegex, OptCasesense, OptUtf, OptRecursive;
extern char FileMask[MAX_MASK_LEN];

extern char OptFontFace[LF_FACESIZE+1];
extern LONG OptFontHeight;
extern int  OptSaveEncoding;

extern LONG WindowWidth;
extern LONG WindowHeight;

void config_init (void);
void config_save (void);
void config_destroy (void);

#endif
