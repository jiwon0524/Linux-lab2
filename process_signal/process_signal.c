#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
volatile sig_atomic_t signal_received = 0;
// 시그널 처리 함수
void signal_handler(int signo) {
    signal_received = signo;
}
int main(void) {
    pid_t pid;
    int status;
    // 출력 버퍼링 방지
    setbuf(stdout, NULL);
    printf("===== Process and Signal Example =====\n");
    // 자식 프로세스 생성
    pid = fork();
    if (pid < 0) {
        perror("fork");
        return 1;
    }
    if (pid == 0) {
        // 자식 프로세스
        printf("[Child] PID: %d, PPID: %d\n",
               getpid(), getppid());
        // SIGUSR1 시그널 처리 함수 등록
        if (signal(SIGUSR1, signal_handler) == SIG_ERR) {
            perror("signal");
            return 1;
        }
        printf("[Child] Waiting for SIGUSR1...\n");
        // 시그널을 받을 때까지 대기
        while (!signal_received) {
            pause();
        }
        printf("[Child] Received signal: %d\n",
               signal_received);
        printf("[Child] Process finished.\n");
        return 0;
    } else {
        // 부모 프로세스
        printf("[Parent] PID: %d\n", getpid());
        printf("[Parent] Child PID: %d\n", pid);
        sleep(2);
        // 자식 프로세스에 시그널 전달
        printf("[Parent] Sending SIGUSR1 to child...\n");
        if (kill(pid, SIGUSR1) == -1) {
            perror("kill");
            return 1;
        }
        // 자식 프로세스 종료 대기
        if (waitpid(pid, &status, 0) == -1) {
            perror("waitpid");
            return 1;
        }
        printf("[Parent] Child process terminated.\n");
    }
    return 0;
}
