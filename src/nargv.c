//============================================================================
// Name         : nargv.c
// Author       : Triston J. Taylor (pc.wiz.tt@gmail.com)
// Version      : 2.0
// Copyright    : (C) Triston J. Taylor 2012. All Rights Reserved
//============================================================================
// Description  : New Argument Vectors
//
//                Parse a shell style string into an argument vector table.
//
//============================================================================
// Public Calls : nargv_free, nargv_parse, nargv_ifs
// Public Type Structs: NARGV
// Public Data: None

/* nargv.h:

#include <stddef.h>

typedef struct NARGV {
    char **argv, *data, *error_message;
    int argc, data_length, error_index, error_code;
} NARGV;

void nargv_free(NARGV* props);
void nargv_ifs(char *nifs);
NARGV *nargv_parse(char *input)

// end-of-file

*/
//============================================================================

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#define ALLOCATE(n,s)  calloc(n,s)
#define FREEMEM(p)     free(p)

enum NARGV_ERRORS  {
  NARGV_ERR_OK = 0,
  NARGV_ERR_NULL_INPUT,
  NARGV_ERR_EMPTY_INPUT,
  NARGV_ERR_UNTERM_DOUBLE,
  NARGV_ERR_UNTERM_SINGLE,
  NARGV_ERR_NO_MEMORY
};

//static char *NARGV_IFS = " \t\n";

typedef struct NARGV {
    char **argv, *data, *error_message;
    int argc, data_length, error_index, error_code;
} NARGV;

void nargv_free(NARGV* props) {
    if (props) {
        FREEMEM(props->data);
        FREEMEM(props->argv);
        FREEMEM(props);
    }
}
/*
void nargv_ifs(char *nifs) {
    if (! nifs) {
        NARGV_IFS = " \t\n";
    } else {
        NARGV_IFS = nifs;
    }
}
*/
static inline int nargv_field_seperator(char seperator) {
//    char *list = NARGV_IFS;
    if (seperator <= 32)  return 1;
    else                  return 0;
#if 0
    if (seperator) {
        while (*list) {
            if (*list++ == seperator) return 1;
        }
        return 0;
    }
    return 1; // null is always a field seperator
#endif
}

NARGV *nargv_parse_len(const char *input, long input_length) {

    NARGV *nvp = ALLOCATE(1, sizeof(NARGV));

    if (NULL == nvp)  {
      return nvp;
    }

    if (! input) {
        nvp->error_code = NARGV_ERR_NULL_INPUT;
        nvp->error_message = "cannot parse null pointer";
        return nvp;
    }

    if (! input_length) {
        nvp->error_code = NARGV_ERR_EMPTY_INPUT;
        nvp->error_message = "cannot parse empty input";
        return nvp;
    }

    int composing_argument = 0;
    long quote = 0;
    long index;
    char look_ahead;

    // FIRST PASS
    // discover how many elements we have, and discover how large a data buffer we need
    for (index = 0; index <= input_length; index++) {

        if (nargv_field_seperator(input[index])) {
            if (composing_argument) {
                // close the argument
                composing_argument = 0;
                nvp->data_length++;
            }
            continue;
        }

        if (! composing_argument) {
            nvp->argc++;
            composing_argument = 1;
        }

        switch (input[index]) {

            /* back slash */
            case '\\':
                // If the sequence is not \' or \" or seperator copy the back slash, and
                // the data
                look_ahead = *(input+index+1);
                if (look_ahead == '"' || look_ahead == '\'' || nargv_field_seperator(look_ahead)) {
                    index++;
                } else {
                    index++;
                    nvp->data_length++;
                }
            break;

            /* double quote */
            case '"':
                quote = index;
                while (input[++index] != '"') {
                    switch (input[index]) {
                        case '\0':
                            nvp->error_index = quote + 1;
                            nvp->error_code = NARGV_ERR_UNTERM_DOUBLE;
                            nvp->error_message = "unterminated double quote";
                            return nvp;
                        break;
                        case '\\':
                            look_ahead = *(input + index + 1);
                            if (look_ahead == '"') {
                                index++;
                            } else {
                                index++;
                                nvp->data_length++;
                            }
                        break;
                    }
                    nvp->data_length++;
                }

                continue;
            break;

            /* single quote */
            case '\'':
                /* copy single quoted data */
                quote = index; // QT used as temp here...
                while (input[++index] != '\'') {
                    if (! input[index]) {
                        // unterminated double quote @ input
                        nvp->error_index = quote + 1;
                        nvp->error_code = NARGV_ERR_UNTERM_SINGLE;
                        nvp->error_message = "unterminated single quote";
                        return nvp;
                    }
                    nvp->data_length++;
                }
                continue;
            break;

        }

        // "record" the data
        nvp->data_length++;

    }

    nvp->argv = ALLOCATE(nvp->argc + 1, sizeof(char *)); // last item should be NULL, so, + 1
    nvp->data = ALLOCATE(nvp->data_length, 1);

    if ((NULL == nvp->argv) || (NULL == nvp->data))  {
        nvp->error_code = NARGV_ERR_NO_MEMORY;
        nvp->error_message = "not enough memory to parse input";
        return nvp;
    }
    nvp->argv[nvp->argc] = NULL; // last item should be NULL

    // SECOND PASS
    composing_argument = 0;
    quote = 0;

    int data_index = 0;
    int arg_index = 0;

    for (index = 0; index <= input_length; index++) {

        if (nargv_field_seperator(input[index])) {
            if (composing_argument) {
                composing_argument = 0;
                nvp->data[data_index++] = '\0';
            }
            continue;
        }

        if (! composing_argument) {
            nvp->argv[arg_index++] = (nvp->data + data_index);
            composing_argument = 1;
        }

        switch (input[index]) {

            /* back slash */
            case '\\':
                // If the sequence is not \' or \" or field seperator copy the backslash
                look_ahead = *(input+index+1);
                if (look_ahead == '"' || look_ahead == '\'' || nargv_field_seperator(look_ahead)) {
                    index++;
                } else {
                    nvp->data[data_index++] = input[index++];
                }
            break;

            /* double quote */
            case '"':
                while (input[++index] != '"') {
                    if (input[index] == '\\') {
                        look_ahead = *(input + index + 1);
                        if (look_ahead == '"') {
                            index++;
                        } else {
                            nvp->data[data_index++] = input[index++];
                        }
                    }
                    nvp->data[data_index++] = input[index];
                }
                continue;
            break;

            /* single quote */
            case '\'':
                /* copy single quoted data */
                while (input[++index] != '\'') {
                    nvp->data[data_index++] = input[index];
                }
                continue;
            break;

        }

        // "record" the data
        nvp->data[data_index++] = input[index];

    }

    return nvp;
}

NARGV *nargv_parse(char *input) {
  if (! input) return NULL;
  return nargv_parse_len (input, strlen (input));
}



#ifdef NARGVTEST
#include <stdio.h>
int main(int argc, char *argv[]) {

    int arg;
    char line_in[4096];

    while (fgets(line_in, 4096, stdin) == line_in) {
        NARGV *nargv = nargv_parse(line_in);
        if (nargv)  {
            if (nargv->error_code) {
                printf("\nnargv parse error: %i: %s: at input column %i\n",
                nargv->error_code, nargv->error_message, nargv->error_index);
            } else {
                printf("\nNARGV Argument Count: %i\n", nargv->argc);
                printf("NARGV Data Length: %i\n\n", nargv->data_length);
                for (arg = 0; arg < nargv->argc; arg++) {
                    printf("argument %i: %s\n", arg, nargv->argv[arg]);
                }
                if (nargv->argv[nargv->argc] != NULL)  {
                  printf ("last argv not NULL!!!\n");
                }
            }
            nargv_free(nargv);
        }
        else {
            printf ("\nNot enough memory for NARGV struct.\n");
        }
    }

}
#endif
