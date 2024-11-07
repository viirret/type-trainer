#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define DEFAULT_CONFIG "/.config/type-trainer/config.txt"
#define DEBUG_CONFIG "../config.txt"

void Config_useDefault(Config* config) {
    const char* fontPath = "/usr/share/fonts/gnu-free/FreeMono.otf";
    config->font = strdup(fontPath);
    config->path_size = 28;
    config->total_words = 5;
    config->font_ok = false;
    config->path_size_ok = false;
    config->total_words_ok = false;
}

// Trim leading and trailing whitespace from a string
void trim(char* str) {
    // Trim leading whitespace
    while (isspace((unsigned char)*str)) str++;

    char* end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) end--;

    // Null-terminate string
    *(end + 1) = '\0';
}

// Function to load config values from a file
int Config_load(Config* config) {
    FILE* file = NULL;

    // Expand `~` in `DEFAULT_CONFIG` to the user's home directory
    char homeConfigPath[256];
    const char* home = getenv("HOME");
    if (home) {
        snprintf(homeConfigPath, sizeof(homeConfigPath), "%s%s", home, DEFAULT_CONFIG);
        file = fopen(homeConfigPath, "r");
    }

    // If the default config is not found, try the debug config path
    if (!file) {
        file = fopen(DEBUG_CONFIG, "r");
    }

    if (!file) {
        printf("Could not find config! Using default values\n");
        Config_useDefault(config);
        return -1;
    }

    char line[256];
    char key[64], value[128];

    // Define the sscanf format string as a variable.
    const char* format = "%63[^=]=%127[^\n]";

    config->font_ok = false;
    config->path_size_ok = false;
    config->total_words_ok = false;

    while (fgets(line, sizeof(line), file)) {
        // Trim newline characters
        line[strcspn(line, "\n")] = 0;
        trim(line);

        // Parse the line into 'key' and 'value'.
        if (sscanf(line, format, key, value) == 2) {
            trim(key);
            trim(value);

            if (strcmp(key, "font") == 0) {
                printf("Loading font: %s\n", value);
                config->font = malloc(strlen(value) + 1);
                if (config->font) {
                    strcpy(config->font, value);
                }
                config->font_ok = true;
            }
            else if (strcmp(key, "path_size") == 0) {
                config->path_size = atoi(value);
                config->path_size_ok = true;
            }
            else if (strcmp(key, "total_words") == 0) {
                config->total_words = atoi(value);
                config->total_words_ok = true;
            }
            else {
                printf("Warning: Skipping invalid line: %s\n", line);
            }
        }
    }

    if (!config->font_ok || !config->path_size_ok || !config->total_words_ok) {
        printf("Error in config, using defaults\n");
        Config_useDefault(config);
        return -1;
    }

    fclose(file);
    return 0;
}

// Function to free the config
void Config_free(Config* config) {
    if (config) {
        free(config->font);
    }
}

