#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "builtin.h"
#define MAX_INPUT 1024
int execute_builtin(char *input)
{
    char buffer[MAX_INPUT];
    char *command;
    char *argument;
    snprintf(buffer, sizeof(buffer), "%s", input);
    command = strtok(buffer, " \t");
    if (command == NULL) {
        return 1;
    }
    // cd 명령어
    if (strcmp(command, "cd") == 0) {
        argument = strtok(NULL, " \t");
        if (argument == NULL) {
            argument = getenv("HOME");
        }
        if (argument == NULL) {
            fprintf(stderr, "cd: HOME is not set\n");
        } else if (chdir(argument) != 0) {
            perror("cd");
        }
        return 1;
    }
    // pwd 명령어
    if (strcmp(command, "pwd") == 0) {
        char cwd[4096];
        if (getcwd(cwd, sizeof(cwd)) != NULL) {
            printf("%s\n", cwd);
        } else {
            perror("pwd");
        }
        return 1;
    }
    // exit 명령어
    if (strcmp(command, "exit") == 0) {
        printf("Exiting Mini Shell...\n");
        exit(0);
    }
    return 0;
}
