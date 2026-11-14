#include <stdio.h>
#include <ctype.h>
#include "parser.h"

int parse_arguments(char *input, char **args, int max_args)
{
    int argc = 0;
    char *src = input;
    char *dst = input;

    while (*src) {
        while (isspace((unsigned char)*src)) {
            src++;
        }

        if (*src == '\0') {
            break;
        }

        if (argc >= max_args - 1) {
            fprintf(stderr, "mini-shell: too many arguments\n");
            args[0] = NULL;
            return -1;
        }

        args[argc++] = dst;
        char quote = 0;

        while (*src) {
            if (quote == 0) {
                if (*src == '\'' || *src == '"') {
                    quote = *src++;
                } else if (isspace((unsigned char)*src)) {
                    break;
                } else {
                    *dst++ = *src++;
                }
            } else {
                if (*src == quote) {
                    quote = 0;
                    src++;
                } else {
                    *dst++ = *src++;
                }
            }
        }

        if (quote != 0) {
            fprintf(stderr, "mini-shell: unmatched quote\n");
            args[0] = NULL;
            return -1;
        }

        char *next = src;

        while (isspace((unsigned char)*next)) {
            next++;
        }

        *dst++ = '\0';
        src = next;
    }

    args[argc] = NULL;
    return argc;
}
