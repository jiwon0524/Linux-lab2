#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <mqueue.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/wait.h>
#define BUFFER_SIZE 256
#define QUEUE_PARENT "/lab2_parent_queue"
#define QUEUE_CHILD  "/lab2_child_queue"
int main(void) {
    mqd_t parent_queue, child_queue;
    struct mq_attr attr;
    pid_t pid;
    char message[BUFFER_SIZE];
    setbuf(stdout, NULL);
    // 메시지 큐 속성 설정
    memset(&attr, 0, sizeof(attr));
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = BUFFER_SIZE;
    // 이전 실행에서 남은 메시지 큐 삭제
    mq_unlink(QUEUE_PARENT);
    mq_unlink(QUEUE_CHILD);
    // 메시지 큐 생성
    parent_queue = mq_open(
        QUEUE_PARENT, O_CREAT | O_RDWR, 0600, &attr);
    if (parent_queue == (mqd_t)-1) {
        perror("mq_open parent");
        return 1;
    }
    child_queue = mq_open(
        QUEUE_CHILD, O_CREAT | O_RDWR, 0600, &attr);
    if (child_queue == (mqd_t)-1) {
        perror("mq_open child");
        mq_close(parent_queue);
        mq_unlink(QUEUE_PARENT);
        return 1;
    }
    // 자식 프로세스 생성
    pid = fork();
    if (pid < 0) {
        perror("fork");
        mq_close(parent_queue);
        mq_close(child_queue);
        mq_unlink(QUEUE_PARENT);
        mq_unlink(QUEUE_CHILD);
        return 1;
    }
    if (pid == 0) {
        // 자식 프로세스
        while (1) {
            ssize_t n = mq_receive(
                parent_queue, message, BUFFER_SIZE, NULL);
            if (n == -1) {
                perror("mq_receive child");
                break;
            }
            message[n] = '\0';
            if (strcmp(message, "exit") == 0)
                break;
            printf("[Child] Received: %s\n", message);
            char reply[BUFFER_SIZE];
            snprintf(reply, sizeof(reply),
                     "Child received: %.230s", message);
            if (mq_send(child_queue, reply,
                        strlen(reply) + 1, 0) == -1) {
                perror("mq_send child");
                break;
            }
        }
        mq_close(parent_queue);
        mq_close(child_queue);
        printf("[Child] Chat ended.\n");
        return 0;
    }
    // 부모 프로세스
    printf("===== POSIX Message Queue Chat =====\n");
    printf("Type a message (exit to quit).\n\n");
    while (1) {
        printf("[Parent] Input: ");
        if (fgets(message, sizeof(message), stdin) == NULL) {
            strcpy(message, "exit");
        } else {
            message[strcspn(message, "\n")] = '\0';
        }
        if (mq_send(parent_queue, message,
                    strlen(message) + 1, 0) == -1) {
            perror("mq_send parent");
            break;
        }
        if (strcmp(message, "exit") == 0)
            break;
        ssize_t n = mq_receive(
            child_queue, message, BUFFER_SIZE, NULL);
        if (n == -1) {
            perror("mq_receive parent");
            break;
        }
        message[n] = '\0';
        printf("[Parent] Received: %s\n\n", message);
    }
    waitpid(pid, NULL, 0);
    // 메시지 큐 닫기 및 삭제
    mq_close(parent_queue);
    mq_close(child_queue);
    mq_unlink(QUEUE_PARENT);
    mq_unlink(QUEUE_CHILD);
    printf("[Parent] Chat ended.\n");
    return 0;
}
