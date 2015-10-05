#ifndef NARGV_H_INCLUDED
#define NARGV_H_INCLUDED

#ifdef __cplusplus
  extern "C" {
#endif

#include <stddef.h>

typedef struct NARGV {
    char **argv, *data, *error_message;
    int argc, data_length, error_index, error_code;
} NARGV;

void nargv_free(NARGV* props);
//void nargv_ifs(char *nifs);
NARGV *nargv_parse(const char *input);
NARGV *nargv_parse_len(const char *input, long input_length);

#ifdef __cplusplus
  }
#endif
#endif
