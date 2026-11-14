#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#include "redirect.h"

int setup_redirection(char **args)
{
    int i = 0;
    int j = 0;

    while (args[i] != NULL) {

        // 출력 리다이렉션
        if (strcmp(args[i], ">") == 0) {
            if (args[i + 1] == NULL) {
                fprintf(stderr, "mini-shell: missing output file\n");
                return -1;
            }

            int fd = open(args[i + 1],
                          O_WRONLY | O_CREAT | O_TRUNC, 0644);

            if (fd < 0) {
                perror("open output");
                return -1;
            }

            if (dup2(fd, STDOUT_FILENO) < 0) {
                perror("dup2 output");
                close(fd);
                return -1;
            }

            close(fd);
            i += 2;
        }

        // 입력 리다이렉션
        else if (strcmp(args[i], "<") == 0) {
            if (args[i + 1] == NULL) {
                fprintf(stderr, "mini-shell: missing input file\n");
                return -1;
            }

            int fd = open(args[i + 1], O_RDONLY);

            if (fd < 0) {
                perror("open input");
                return -1;
            }

            if (dup2(fd, STDIN_FILENO) < 0) {
                perror("dup2 input");
                close(fd);
                return -1;
            }

            close(fd);
            i += 2;
        }

        else {
            args[j++] = args[i++];
        }
    }

    args[j] = NULL;

    if (j == 0) {
        fprintf(stderr, "mini-shell: missing command\n");
        return -1;
    }

    return 0;
}
