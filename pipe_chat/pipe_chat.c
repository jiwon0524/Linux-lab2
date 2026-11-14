#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#define BUFFER_SIZE 256
int main(void) {
    int parent_to_child[2];
    int child_to_parent[2];
    pid_t pid;
    char message[BUFFER_SIZE];
    setbuf(stdout, NULL);
    // 양방향 통신을 위한 파이프 생성
    if (pipe(parent_to_child) == -1) {
        perror("pipe");
        return 1;
    }
    if (pipe(child_to_parent) == -1) {
        perror("pipe");
        close(parent_to_child[0]);
        close(parent_to_child[1]);
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
        while (1) {
            ssize_t n = read(parent_to_child[0],
                             message, sizeof(message) - 1);
            if (n <= 0)
                break;
            message[n] = '\0';
            if (strcmp(message, "exit") == 0)
                break;
            printf("[Child] Received: %s\n", message);
            // 자식 프로세스의 응답
            char reply[BUFFER_SIZE];
            snprintf(reply, sizeof(reply),
                     "Child received: %.230s", message);
            if (write(child_to_parent[1], reply,
                      strlen(reply) + 1) == -1) {
                perror("child write");
                break;
            }
        }
        close(parent_to_child[0]);
        close(child_to_parent[1]);
        printf("[Child] Chat ended.\n");
        return 0;
    }
    // 부모 프로세스
    close(parent_to_child[0]);
    close(child_to_parent[1]);
    printf("===== Pipe Chat Program =====\n");
    printf("Type a message (exit to quit).\n\n");
    while (1) {
        printf("[Parent] Input: ");
        if (fgets(message, sizeof(message), stdin) == NULL)
            break;
        message[strcspn(message, "\n")] = '\0';
        if (write(parent_to_child[1], message,
                  strlen(message) + 1) == -1) {
            perror("parent write");
            break;
        }
        if (strcmp(message, "exit") == 0)
            break;
        ssize_t n = read(child_to_parent[0],
                         message, sizeof(message) - 1);
        if (n <= 0)
            break;
        message[n] = '\0';
        printf("[Parent] Received: %s\n\n", message);
    }
    close(parent_to_child[1]);
    close(child_to_parent[0]);
    waitpid(pid, NULL, 0);
    printf("[Parent] Chat ended.\n");
    return 0;
}
