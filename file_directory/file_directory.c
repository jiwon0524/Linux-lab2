#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>

int main(void) {
    int fd;
    char buffer[100] = {0};
    const char *message = "Hello Linux File System!";
    DIR *dir;
    struct dirent *entry;

    // 1. 디렉토리 생성
    if (mkdir("test_dir", 0755) == -1) {
        perror("mkdir");
        return 1;
    }
    printf("1. 디렉토리 생성 완료\n");

    // 2. 파일 생성 및 열기
    fd = open("test_dir/test.txt", O_CREAT | O_WRONLY | O_TRUNC, 0644);
    if (fd == -1) {
        perror("open");
        return 1;
    }

    // 3. 파일에 데이터 쓰기
    if (write(fd, message, strlen(message)) == -1) {
        perror("write");
        close(fd);
        return 1;
    }
    close(fd);
    printf("2. 파일 생성 및 쓰기 완료\n");

    // 4. 파일 읽기
    fd = open("test_dir/test.txt", O_RDONLY);
    if (fd == -1) {
        perror("open");
        return 1;
    }

    ssize_t bytes = read(fd, buffer, sizeof(buffer) - 1);
    if (bytes == -1) {
        perror("read");
        close(fd);
        return 1;
    }
    buffer[bytes] = '\0';
    close(fd);

    printf("3. 파일 읽기 결과: %s\n", buffer);

    // 5. 디렉토리 열기 및 목록 출력
    dir = opendir("test_dir");
    if (dir == NULL) {
        perror("opendir");
        return 1;
    }

    printf("4. 디렉토리 목록:\n");
    while ((entry = readdir(dir)) != NULL) {
        printf("   %s\n", entry->d_name);
    }
    closedir(dir);

    return 0;
}
