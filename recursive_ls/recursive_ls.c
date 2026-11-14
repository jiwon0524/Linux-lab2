#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <limits.h>

void list_directory(const char *path) {
    DIR *dir;
    struct dirent *entry;
    dir = opendir(path);
    if (dir == NULL) {
        perror(path);
        return;
    }
    printf("\n%s:\n", path);
    // 현재 디렉토리의 파일과 폴더 출력
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 ||
            strcmp(entry->d_name, "..") == 0) {
            continue;
        }
        printf("%s  ", entry->d_name);
    }
    printf("\n");
    rewinddir(dir);
    // 하위 디렉토리를 재귀적으로 탐색
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 ||
            strcmp(entry->d_name, "..") == 0) {
            continue;
        }
        char full_path[PATH_MAX];
        struct stat file_stat;
        int n = snprintf(full_path, sizeof(full_path),
                         "%s/%s", path, entry->d_name);
        if (n < 0 || (size_t)n >= sizeof(full_path)) {
            fprintf(stderr, "경로가 너무 깁니다.\n");
            continue;
        }
        if (lstat(full_path, &file_stat) == -1) {
            perror(full_path);
            continue;
        }
        if (S_ISDIR(file_stat.st_mode)) {
            list_directory(full_path);
        }
    }
    closedir(dir);
}
int main(int argc, char *argv[]) {
    const char *path = ".";
    if (argc > 1) {
        path = argv[1];
    }
    printf("재귀적 디렉토리 탐색 프로그램\n");
    list_directory(path);
    return 0;
}
