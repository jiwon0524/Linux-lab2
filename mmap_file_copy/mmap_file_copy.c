#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/mman.h>
int main(int argc, char *argv[]) {
    int src_fd, dst_fd;
    struct stat file_stat;
    void *src_map, *dst_map;
    size_t file_size;
    if (argc != 3) {
        printf("Usage: %s <source> <destination>\n", argv[0]);
        return 1;
    }
    // 원본 파일 열기
    src_fd = open(argv[1], O_RDONLY);
    if (src_fd == -1) {
        perror("open source");
        return 1;
    }
    // 원본 파일 크기 확인
    if (fstat(src_fd, &file_stat) == -1) {
        perror("fstat");
        close(src_fd);
        return 1;
    }
    if (!S_ISREG(file_stat.st_mode) || file_stat.st_size == 0) {
        fprintf(stderr, "Source must be a non-empty regular file.\n");
        close(src_fd);
        return 1;
    }
    file_size = (size_t)file_stat.st_size;
    // 복사 대상 파일 생성
    dst_fd = open(argv[2], O_RDWR | O_CREAT, 0644);
    if (dst_fd == -1) {
        perror("open destination");
        close(src_fd);
        return 1;
    }
    // 원본과 대상이 동일한 파일인지 확인
    struct stat dst_stat;
    if (fstat(dst_fd, &dst_stat) == -1) {
        perror("fstat destination");
        close(src_fd);
        close(dst_fd);
        return 1;
    }
    if (file_stat.st_dev == dst_stat.st_dev &&
        file_stat.st_ino == dst_stat.st_ino) {
        fprintf(stderr, "Source and destination are the same file.\n");
        close(src_fd);
        close(dst_fd);
        return 1;
    }
    // 복사 대상 파일 크기 설정
    if (ftruncate(dst_fd, file_stat.st_size) == -1) {
        perror("ftruncate");
        close(src_fd);
        close(dst_fd);
        return 1;
    }
    // 원본 파일 메모리 매핑
    src_map = mmap(NULL, file_size,
                   PROT_READ, MAP_PRIVATE, src_fd, 0);
    if (src_map == MAP_FAILED) {
        perror("mmap source");
        close(src_fd);
        close(dst_fd);
        return 1;
    }
    // 대상 파일 메모리 매핑
    dst_map = mmap(NULL, file_size,
                   PROT_READ | PROT_WRITE,
                   MAP_SHARED, dst_fd, 0);
    if (dst_map == MAP_FAILED) {
        perror("mmap destination");
        munmap(src_map, file_size);
        close(src_fd);
        close(dst_fd);
        return 1;
    }
    // 메모리 영역을 이용한 파일 복사
    memcpy(dst_map, src_map, file_size);
    // 변경 내용 파일에 반영
    if (msync(dst_map, file_size, MS_SYNC) == -1) {
        perror("msync");
        munmap(src_map, file_size);
        munmap(dst_map, file_size);
        close(src_fd);
        close(dst_fd);
        return 1;
    }
    // 메모리 매핑 해제
    munmap(src_map, file_size);
    munmap(dst_map, file_size);
    // 파일 닫기
    close(src_fd);
    close(dst_fd);
    printf("===== mmap File Copy =====\n");
    printf("Source      : %s\n", argv[1]);
    printf("Destination : %s\n", argv[2]);
    printf("File size   : %zu bytes\n", file_size);
    printf("File copied successfully.\n");
    return 0;
}
