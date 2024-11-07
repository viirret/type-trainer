#ifndef CONFIG_H
#define CONFIG_H

#include <stdbool.h>

typedef struct {
    char* font;
    int path_size;
    int total_words;

    bool font_ok;
    bool path_size_ok;
    bool total_words_ok;
} Config;

int Config_load(Config* config);
void Config_free(Config* config);
void Config_useDefault(Config* config);

#endif
