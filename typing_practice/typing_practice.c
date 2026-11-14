#include <stdio.h>
#include <string.h>
#include <time.h>

#define SENTENCE_COUNT 5
#define MAX_LENGTH 200

int main(void) {
    const char *sentences[SENTENCE_COUNT] = {
        "Linux is an open source operating system.",
        "C programming is useful for system development.",
        "Practice makes perfect.",
        "Operating systems manage computer resources.",
        "Learning Linux is fun and interesting."
    };

    char input[MAX_LENGTH];
    int total_errors = 0;
    int total_chars = 0;

    printf("===== Typing Practice Program =====\n");
    printf("Type the following sentences.\n");
    printf("Press Enter to submit each sentence.\n\n");

    printf("Press Enter to start...");
    getchar();

    time_t start_time = time(NULL);

    for (int i = 0; i < SENTENCE_COUNT; i++) {
        printf("\n[%d/%d]\n", i + 1, SENTENCE_COUNT);
        printf("Sentence: %s\n", sentences[i]);
        printf("Input   : ");

        if (fgets(input, sizeof(input), stdin) == NULL) {
            printf("\nInput ended.\n");
            return 1;
        }

        input[strcspn(input, "\n")] = '\0';

        int expected_len = (int)strlen(sentences[i]);
        int input_len = (int)strlen(input);
        int errors = 0;

        int max_len = expected_len > input_len
                      ? expected_len : input_len;

        for (int j = 0; j < max_len; j++) {
            if (j >= expected_len ||
                j >= input_len ||
                sentences[i][j] != input[j]) {
                errors++;
            }
        }

        total_errors += errors;
        total_chars += input_len;

        printf("Errors  : %d\n", errors);
    }

    time_t end_time = time(NULL);

    double elapsed = difftime(end_time, start_time);

    if (elapsed < 1.0) {
        elapsed = 1.0;
    }

    double minutes = elapsed / 60.0;
    double cpm = total_chars / minutes;

    printf("\n===== Typing Result =====\n");
    printf("Total sentences : %d\n", SENTENCE_COUNT);
    printf("Total characters: %d\n", total_chars);
    printf("Total errors    : %d\n", total_errors);
    printf("Elapsed time    : %.1f seconds\n", elapsed);
    printf("Average CPM     : %.2f\n", cpm);

    return 0;
}
