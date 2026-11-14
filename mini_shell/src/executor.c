#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include "executor.h"
#include "signals.h"
#include "redirect.h"
#include "parser.h"
#define MAX_ARGS 64
static void reap_background_processes(void)
{
    int status;
    pid_t pid;
    while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
        printf("[Background] Process %ld finished.\n", (long)pid);
    }
}
void execute_command(char *input)
{
    char *args[MAX_ARGS];
    int argc = 0;
    int background = 0;
    reap_background_processes();
    argc = parse_arguments(input, args, MAX_ARGS);

	if (argc < 0) {
    	return;
	}
    if (argc == 0) {
        return;
    }
    if (strcmp(args[argc - 1], "&") == 0) {
        background = 1;
        args[--argc] = NULL;
    }
    if (argc == 0) {
        fprintf(stderr, "mini-shell: missing command\n");
        return;
    }
    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        return;
    }
    if (pid == 0) {
    setup_child_signals();

    // 입출력 리다이렉션 설정
    if (setup_redirection(args) < 0) {
        _exit(1);
    }

    execvp(args[0], args);

    perror("mini-shell");
    _exit(127);
}

    if (background) {
        printf("[Background] Process started: %ld\n", (long)pid);
    } else {
        int status;

        // 자식 프로세스가 종료되거나 정지할 때까지 대기
        while (waitpid(pid, &status, WUNTRACED) == -1) {
            if (errno != EINTR) {
                perror("waitpid");
                return;
            }
        }

        if (WIFSIGNALED(status)) {
            printf("\n[Signal] Process %ld terminated by signal %d\n",
                   (long)pid, WTERMSIG(status));
        } else if (WIFSTOPPED(status)) {
            printf("\n[Signal] Process %ld stopped by signal %d\n",
                   (long)pid, WSTOPSIG(status));
        }
    }
}
