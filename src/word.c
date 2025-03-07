#include "word.h"

#include <string.h>
#include <time.h>
#include <stdlib.h>

int containsNonAscii(const char* word) {
    while (*word) {
        if ((unsigned char)(*word) > 127) {
            return 1;
        }
        word++;
    }
    return 0;
}

void Word_init(Word* word, const char* dictionary_path) {
    fflush(stdout);
    word->total_lines = 0;
    word->lines = NULL;

    // Open the dictionary file
    FILE* file = fopen(dictionary_path, "r");
    if (!file) {
        fprintf(stderr, "Failed to open dictionary file in path %s\n", dictionary_path);
        return;
    }

    int line_count = 0;
    char line[256];

    while (fgets(line, sizeof(line), file)) {
        if (strlen(line) < 10) {
            line_count++;
        }
    }

    // Allocate memory for lines array based on line_count
    word->lines = malloc(sizeof(char*) * line_count);
    if (!word->lines) {
        fprintf(stderr, "Memory allocation for lines array failed.\n");
        fclose(file);
        return;
    }

    // Reset the file pointer to the beginning
    rewind(file);

    // Second pass: Store lines shorter than 10 characters
    int index = 0;
    while (fgets(line, sizeof(line), file)) {
        // Remove newline character
        line[strcspn(line, "\n")] = '\0';

        if (strlen(line) < 10) {
            if (index >= line_count) {
                break;
            }

            word->lines[index] = strdup(line);
            if (!word->lines[index]) {
                fprintf(stderr, "Memory allocation for line %d failed.\n", index);
                fclose(file);
                return;
            }
            index++;
        }
    }

    word->total_lines = line_count;
    printf("Total lines in %s: %d\n", dictionary_path, word->total_lines);
    fclose(file);
}

void Word_destroy(Word* word) {
    for (int i = 0; i < word->total_lines; i++) {
        free(word->lines[i]);
    }
    free(word->lines);
    word->lines = NULL;
}

int randomLine(int max) {
    return rand() % max;
}

char* Word_getSentence(Word* word, int n) {
    if (word->total_lines == 0) {
        fprintf(stderr, "No lines available in Word structure.\n");
        return NULL;
    }
    static char sentence[512];
    sentence[0] = '\0';

    srand((unsigned) time(NULL));

    int word_count = 0;
    while (word_count < n) {
        int random_index = randomLine(word->total_lines);
        if (word->lines[random_index] == NULL) {
            fprintf(stderr, "Null line encountered at index %d.\n", random_index);
            continue;
        }

        // Skip non-ASCII words
        if (containsNonAscii(word->lines[random_index])) {
            continue;
        }

        // Concatenate word to sentence
        strcat(sentence, word->lines[random_index]);

        // Add space if not the last word
        if (word_count < n - 1) {
            strcat(sentence, " ");
        }
        word_count++;
    }

    printf("Sentence: %s\n", sentence);
    return sentence;
}
