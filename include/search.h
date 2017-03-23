#ifndef SEARCH_H_INCLUDED
#define SEARCH_H_INCLUDED
#ifdef __cplusplus
  extern "C" {
#endif

#define FBUF_SIZE_MB   32
#define FBUF_SIZE      (FBUF_SIZE_MB)*1024L*1024L
extern char fbuf[FBUF_SIZE];
extern char loaded_file[_MAX_PATH];
extern size_t loaded_len;
extern int  loaded_flags;

void search_init (void);
void search_clear (void);
void search_destroy (void);
int  search_is_in_progress (void);

void search_in_path (char *lookfor, const char *path, const char *mask);

//int load_file_to_fbuf (const char *fullname, int for_search);

int show_and_highlight_file_ex (const char *fullname);

#define FLAG_CRLF  1
#define FLAG_BIN   2
#define FLAG_UTF8  4

#define LINE_LEN   1024

#ifdef __cplusplus
  }
#endif
#endif