#ifndef SHLINK_H_INCLUDED
#define SHLINK_H_INCLUDED

#ifdef __cplusplus
  extern "C" {
#endif

  HRESULT CreateShortCut(LPSTR pszTargetfile, LPSTR pszTargetargs,
                                LPSTR pszLinkfile, LPSTR pszDescription,
                                int iShowmode, LPSTR pszCurdir,
                                LPSTR pszIconfile, int iIconindex);

#ifdef __cplusplus
  }
#endif
#endif
