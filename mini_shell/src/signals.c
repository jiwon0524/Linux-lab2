#define _POSIX_C_SOURCE 200809L

#include <signal.h>
#include <stdio.h>

#include "signals.h"

// 셸 프로세스의 시그널 처리
void setup_shell_signals(void)
{
    struct sigaction sa = {0};

    // 셸은 Ctrl+C, Ctrl+Z로 종료되거나 정지되지 않음
    sa.sa_handler = SIG_IGN;
    sigemptyset(&sa.sa_mask);

    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTSTP, &sa, NULL);
}

// 자식 프로세스의 시그널 처리
void setup_child_signals(void)
{
    struct sigaction sa = {0};

    // 자식 프로세스는 기본 시그널 동작 복원
    sa.sa_handler = SIG_DFL;
    sigemptyset(&sa.sa_mask);

    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTSTP, &sa, NULL);
}

