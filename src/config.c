#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define DEFAULT_CONFIG "/.config/type-trainer/config.txt"
#define DEBUG_CONFIG "../config.txt"

void modifyItemString(ConfigItem* item, const char* value) {
    item->value.str_value = malloc(strlen(value) + 1);
    if (item->value.str_value) {
        strcpy(item->value.str_value, value);
    }
    else {
        printf("Unsuccessfull malloc");
    }
    item->type = CONFIG_TYPE_STRING;
    item->is_set = true;
}

void modifyItemInt(ConfigItem* item, int value) {
    item->value.int_value = value;
    item->type = CONFIG_TYPE_INT;
    item->is_set = true;
}

void modifyItemBoolean(ConfigItem* item, bool value) {
    item->value.boolean_value = value;
    item->type = CONFIG_TYPE_BOOLEAN;
    item->is_set = true;
}

void modifyItemColor(ConfigItem* item, SDL_Color value) {
    item->value.color_value = value;
    item->type = CONFIG_TYPE_COLOR;
    item->is_set = true;
}

const char* configNameTypeToString(ConfigNameType type) {
    switch (type) {
        case CONFIG_NAME_FONT: return "font";
        case CONFIG_NAME_PATH_SIZE: return "path_size";
        case CONFIG_NAME_TOTAL_WORDS: return "total_words";
        case CONFIG_NAME_BACKGROUND_COLOR: return "background_color";
        case CONFIG_NAME_ADVANCE_ON_FAILURE: return "advance_on_failure";
    }
}

void Config_useDefault(Config* config) {
    Config_useDefaultForItem(config, config->font);
    Config_useDefaultForItem(config, config->path_size);
    Config_useDefaultForItem(config, config->total_words);
    Config_useDefaultForItem(config, config->background_color);
}

void Config_useDefaultForItem(Config* config, ConfigItem configItem) {
    switch (configItem.name) {
        case CONFIG_NAME_FONT:
            config->font.name = CONFIG_NAME_FONT;
            const char* font_path = "/usr/share/fonts/gnu-free/FreeMono.otf";
            config->font.value.str_value = strdup(font_path);
            config->font.type = CONFIG_TYPE_STRING;
            config->font.is_set = true;
            break;
        case CONFIG_NAME_PATH_SIZE:
            config->path_size.name = CONFIG_NAME_PATH_SIZE;
            config->path_size.value.int_value = 28;
            config->path_size.type = CONFIG_TYPE_INT;
            config->path_size.is_set = true;
            break;
        case CONFIG_NAME_TOTAL_WORDS:
            config->total_words.name = CONFIG_NAME_TOTAL_WORDS;
            config->total_words.value.int_value = 30;
            config->total_words.type = CONFIG_TYPE_INT;
            config->total_words.is_set = true;
            break;
        case CONFIG_NAME_BACKGROUND_COLOR:
            config->background_color.name = CONFIG_NAME_BACKGROUND_COLOR;
            config->background_color.value.color_value = (SDL_Color){0, 255, 0, 255};
            config->background_color.type = CONFIG_TYPE_COLOR;
            config->background_color.is_set = true;
        case CONFIG_NAME_ADVANCE_ON_FAILURE:
            config->advance_on_failure.name = CONFIG_NAME_ADVANCE_ON_FAILURE;
            config->advance_on_failure.value.boolean_value = true;
            config->advance_on_failure.type = CONFIG_TYPE_BOOLEAN;
            config->background_color.is_set = true;
            break;
    }
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

void loadString(ConfigItem* item, char* key, const char* value) {
    const char* item_name = configNameTypeToString(item->name);
    if (strcmp(key, item_name)== 0) {
        modifyItemString(item, value);
    }
}

bool loadBool(ConfigItem* item, char* key, const char* value) {
    const char* item_name = configNameTypeToString(item->name);
    if (strcmp(key, item_name) == 0) {
        if (strcmp(value, "true") == 0) {
            modifyItemBoolean(item, true);
            return true;
        }
        else if (strcmp(value, "false") == 0) {
            modifyItemBoolean(item, false);
            return true;
        }
        else if (strcmp(value, "1") == 0) {
            modifyItemBoolean(item, true);
            return true;
        }
        else if (strcmp(value, "0") == 0) {
            modifyItemBoolean(item, false);
            return true;
        }
        else {
            printf("Undefined argument");
            return false;
        }
    }
    return false;
}

void loadInt(ConfigItem* item, char* key, const char* value) {
    const char* item_name = configNameTypeToString(item->name);
    if (strcmp(key, item_name) == 0) {
        modifyItemInt(item, atoi(value));
    }
}

void loadColor(ConfigItem* item, char* key, const char* value) {
    const char* item_name = configNameTypeToString(item->name);
    if (strcmp(key, item_name) == 0) {
        int r, g, b, a;
        if (sscanf(value, "%d,%d,%d,%d", &r, &g, &b, &a) == 4) {
            item->value.color_value = (SDL_Color){.r = r, .g = g, .b = b, .a = a};
            item->is_set = true;
        }
        else {
            printf("Invalid format for background color. Expected format: R,G,B,A\n");
        }
    }
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

    config->font.is_set = false;
    config->path_size.is_set = false;
    config->total_words.is_set = false;
    config->background_color.is_set = false;

    while (fgets(line, sizeof(line), file)) {
        // Trim newline characters
        line[strcspn(line, "\n")] = 0;
        trim(line);

        // Parse the line into 'key' and 'value'.
        if (sscanf(line, format, key, value) == 2) {
            trim(key);
            trim(value);

            if (strcmp(key, "font") == 0) {
                config->font.name = CONFIG_NAME_FONT;
                loadString(&config->font, key, value);
            }
            else if (strcmp(key, "path_size") == 0) {
                config->path_size.name = CONFIG_NAME_PATH_SIZE;
                loadInt(&config->path_size, key, value);
            }
            else if (strcmp(key, "total_words") == 0) {
                config->total_words.name = CONFIG_NAME_TOTAL_WORDS;
                loadInt(&config->total_words, key, value);
            }
            else if (strcmp(key, "background_color") == 0) {
                config->background_color.name = CONFIG_NAME_BACKGROUND_COLOR;
                loadColor(&config->background_color, key, value);
            }
            else if (strcmp(key, "advance_on_failure") == 0) {
                config->advance_on_failure.name = CONFIG_NAME_ADVANCE_ON_FAILURE;
                loadBool(&config->advance_on_failure, key, value);
            }
            else {
                printf("Warning: Skipping invalid line: %s\n", line);
            }
        }
    }

    if (!config->font.is_set) {
        printf("Config font not set, using default");
        Config_useDefaultForItem(config, config->font);
    }

    if (!config->path_size.is_set) {
        printf("Config path size not set, using default");
        Config_useDefaultForItem(config, config->path_size);
    }

    if (!config->total_words.is_set) {
        printf("Config total words not set, using default");
        Config_useDefaultForItem(config, config->total_words);
    }

    if (!config->background_color.is_set) {
        printf("Config background color not set, using default");
        Config_useDefaultForItem(config, config->background_color);
    }

    fclose(file);
    return 0;
}

// Function to free the config
void Config_destroy(Config* config) {
    if (config) {
        free(config->font.value.str_value);
    }
}

