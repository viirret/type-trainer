#include "word.h"

#include <string.h>
#include <time.h>
#include <stdlib.h>

void Word_init(Word* word) {
    word->total_lines = 0;
    word->lines = NULL;

    // Open the dictionary file
    FILE* file = fopen("/usr/share/dict/american-english", "r");
    if (!file) {
        fprintf(stderr, "Failed to open dictionary file.\n");
        return;
    }

    int line_count = 0;
    char line[256];

    // First pass: Count lines meeting the length condition
    while (fgets(line, sizeof(line), file)) {
        if (strlen(line) < 10) {
            line_count++;
        }
    }

    // Allocate memory based on counted lines
    word->lines = malloc(sizeof(char*) * line_count);
    if (!word->lines) {
        fprintf(stderr, "Memory allocation failed for lines array.\n");
        fclose(file);
        return;
    }

    // Rewind and read lines for the second pass
    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\n")] = '\0';  // Remove newline character

        if (strlen(line) < 10) {
            word->lines[word->total_lines] = strdup(line);
            if (!word->lines[word->total_lines]) {
                fprintf(stderr, "Memory allocation failed for line copy.\n");
                break;
            }
            word->total_lines++;
        }
    }
    fclose(file);  // Close the file after reading
}

void Word_destroy(Word* word) {
    for (int i = 0; i < word->total_lines; i++) {
        free(word->lines[i]);
    }
    free(word->lines);
    word->lines = NULL;
}

char* Word_getSentence(Word* word, int n) {
    static char sentence[512];
    sentence[0] = '\0';

    srand(time(NULL));
    for (int i = 0; i < n; i++) {
        int randomIndex = rand() % word->total_lines;
        strcat(sentence, word->lines[randomIndex]);
        if (i < n - 1) {
            strcat(sentence, " ");
        }
    }
    return sentence;
}
