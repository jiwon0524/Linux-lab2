#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>

#include "pipeline.h"
#include "signals.h"
#include "redirect.h"
#include "parser.h"

#define MAX_COMMANDS 16
#define MAX_ARGS 64

int execute_pipeline(char *input)
{
    char *commands[MAX_COMMANDS];
    pid_t pids[MAX_COMMANDS];

    int command_count = 0;
    int previous_fd = -1;
    int started = 0;

    char *start = input;
    char quote = 0;

    // 따옴표 밖에 있는 파이프 기호만 명령어 구분자로 처리
    for (char *p = input; ; p++) {

        if (*p == '\0' && quote != 0) {
            fprintf(stderr, "mini-shell: unmatched quote\n");
            return 1;
        }

        if (*p == '\'' || *p == '"') {
            if (quote == 0) {
                quote = *p;
            } else if (quote == *p) {
                quote = 0;
            }
        }

        if ((*p == '|' && quote == 0) || *p == '\0') {

            char separator = *p;

            if (command_count >= MAX_COMMANDS) {
                fprintf(stderr, "mini-shell: too many commands\n");
                return 1;
            }

            *p = '\0';
            commands[command_count++] = start;

            if (separator == '\0') {
                break;
            }

            start = p + 1;
        }
    }

    // 빈 명령어 검사
    for (int i = 0; i < command_count; i++) {

        char *p = commands[i];

        while (*p == ' ' || *p == '\t') {
            p++;
        }

        if (*p == '\0') {
            fprintf(stderr, "mini-shell: invalid pipeline\n");
            return 1;
        }
    }

    // 각 명령어에 대해 자식 프로세스 생성
    for (int i = 0; i < command_count; i++) {

        int pipefd[2] = {-1, -1};

        // 마지막 명령어를 제외하고 파이프 생성
        if (i < command_count - 1) {

            if (pipe(pipefd) == -1) {
                perror("pipe");
                goto cleanup;
            }
        }

        pid_t pid = fork();

        if (pid < 0) {

            perror("fork");

            if (pipefd[0] != -1) {
                close(pipefd[0]);
            }

            if (pipefd[1] != -1) {
                close(pipefd[1]);
            }

            goto cleanup;
        }

        if (pid == 0) {

            // 자식 프로세스 시그널 기본 동작 복원
            setup_child_signals();

            // 이전 파이프를 표준 입력에 연결
            if (previous_fd != -1) {

                if (dup2(previous_fd, STDIN_FILENO) == -1) {
                    perror("dup2 input");
                    _exit(1);
                }
            }

            // 다음 파이프를 표준 출력에 연결
            if (pipefd[1] != -1) {

                if (dup2(pipefd[1], STDOUT_FILENO) == -1) {
                    perror("dup2 output");
                    _exit(1);
                }
            }

            // 사용하지 않는 파일 디스크립터 닫기
            if (previous_fd != -1) {
                close(previous_fd);
            }

            if (pipefd[0] != -1) {
                close(pipefd[0]);
            }

            if (pipefd[1] != -1) {
                close(pipefd[1]);
            }

            // 명령어 인자 분석
            char *args[MAX_ARGS];

            int argc = parse_arguments(
                commands[i],
                args,
                MAX_ARGS
            );

            if (argc <= 0) {
                _exit(1);
            }

            // 입출력 리다이렉션 처리
            if (setup_redirection(args) < 0) {
                _exit(1);
            }

            // 외부 명령어 실행
            execvp(args[0], args);

            perror("execvp");
            _exit(127);
        }

        // 부모 프로세스
        pids[started++] = pid;

        if (previous_fd != -1) {
            close(previous_fd);
        }

        if (pipefd[1] != -1) {
            close(pipefd[1]);
        }

        previous_fd = pipefd[0];
    }

    // 마지막 파이프 닫기
    if (previous_fd != -1) {
        close(previous_fd);
        previous_fd = -1;
    }

    // 모든 자식 프로세스 종료 대기
    for (int i = 0; i < started; i++) {

        int status;

        while (waitpid(pids[i], &status, WUNTRACED) == -1) {

            if (errno != EINTR) {
                perror("waitpid");
                break;
            }
        }
    }

    return 0;

cleanup:

    if (previous_fd != -1) {
        close(previous_fd);
    }

    // 오류 발생 시 생성된 자식 프로세스 정리
    for (int i = 0; i < started; i++) {

        int status;

        while (waitpid(pids[i], &status, 0) == -1) {

            if (errno != EINTR) {
                perror("waitpid");
                break;
            }
        }
    }

    return 1;
}
