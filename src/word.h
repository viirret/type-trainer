#ifndef WORD_H
#define WORD_H

#include <stdio.h>

typedef struct {
    char** lines;
    int total_lines;
    FILE* file;
} Word;

// Allocate dictionary context.
void Word_init(Word* word, const char* dictionary_path);

// Deallocate dictionary context.
void Word_destroy(Word* word);

// Get random sentence from the dictionary file.
char* Word_getSentence(Word* word, int n);

#endif
