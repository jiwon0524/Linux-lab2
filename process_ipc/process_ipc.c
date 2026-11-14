#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#define BUFFER_SIZE 100
int main(void) {
    int parent_to_child[2];
    int child_to_parent[2];
    pid_t pid;
    char buffer[BUFFER_SIZE];
    setbuf(stdout, NULL);
    // 두 개의 파이프 생성
    if (pipe(parent_to_child) == -1 ||
        pipe(child_to_parent) == -1) {
        perror("pipe");
        return 1;
    }
    // 자식 프로세스 생성
    pid = fork();
    if (pid < 0) {
        perror("fork");
        return 1;
    }
    if (pid == 0) {
        // 자식 프로세스
        close(parent_to_child[1]);
        close(child_to_parent[0]);
        // 부모가 보낸 메시지 수신
        ssize_t n = read(parent_to_child[0],
                         buffer, sizeof(buffer) - 1);
        if (n <= 0) {
            perror("child read");
            return 1;
        }
        buffer[n] = '\0';
        printf("[Child] Received: %s\n", buffer);
        // 부모에게 응답 메시지 전송
        const char *reply = "Hello Parent!";
        if (write(child_to_parent[1], reply,
                  strlen(reply) + 1) == -1) {
            perror("child write");
            return 1;
        }
        printf("[Child] Sent: %s\n", reply);
        close(parent_to_child[0]);
        close(child_to_parent[1]);
        return 0;
    } else {
        // 부모 프로세스
        close(parent_to_child[0]);
        close(child_to_parent[1]);
        const char *message = "Hello Child!";
        // 자식에게 메시지 전송
        if (write(parent_to_child[1], message,
                  strlen(message) + 1) == -1) {
            perror("parent write");
            return 1;
        }
        printf("[Parent] Sent: %s\n", message);
        // 자식의 응답 메시지 수신
        ssize_t n = read(child_to_parent[0],
                         buffer, sizeof(buffer) - 1);
        if (n <= 0) {
            perror("parent read");
            return 1;
        }
        buffer[n] = '\0';
        printf("[Parent] Received: %s\n", buffer);
        close(parent_to_child[1]);
        close(child_to_parent[0]);
        waitpid(pid, NULL, 0);
        printf("[Parent] Child process finished.\n");
    }
    return 0;
}
