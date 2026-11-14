#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pipeline.h"
#include "executor.h"
#include "builtin.h"
#include "signals.h"

#define MAX_INPUT 1024

int main(void)
{
    char input[MAX_INPUT];

    // 셸 시그널 처리 설정
    setup_shell_signals();

    printf("===== Linux Mini Shell =====\n");
    printf("Type 'exit' to quit.\n\n");

    while (1) {
        printf("mini-shell> ");
        fflush(stdout);

        if (fgets(input, sizeof(input), stdin) == NULL) {
            printf("\n");
            break;
        }

        input[strcspn(input, "\n")] = '\0';

        if (strlen(input) == 0) {
            continue;
        }

        // 파이프 명령어 처리
	if (strchr(input, '|') != NULL) {
    	execute_pipeline(input);
    	continue;
	}

	// 내장 명령어 처리
	if (execute_builtin(input)) {
    	continue;
	}

	// 일반 명령어 실행
	execute_command(input);
    	}

    return 0;
}
