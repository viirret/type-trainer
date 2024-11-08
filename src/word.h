#ifndef WORD_H
#define WORD_H

#include <stdio.h>

typedef struct {
    char** lines;
    int total_lines;
    FILE* file;
} Word;

// Word functions
void Word_init(Word* word, const char* dictionary_path);
void Word_destroy(Word* word);
char* Word_getSentence(Word* word, int n);

#endif
