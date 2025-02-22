#include "config.h"

#include "config_file.h"

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

static void modifyItemString(ConfigItem* item, const char* value) {
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

static void modifyItemInt(ConfigItem* item, int value) {
    item->value.int_value = value;
    item->type = CONFIG_TYPE_INT;
    item->is_set = true;
}

static void modifyItemBoolean(ConfigItem* item, bool value) {
    item->value.boolean_value = value;
    item->type = CONFIG_TYPE_BOOLEAN;
    item->is_set = true;
}

static void modifyItemColor(ConfigItem* item, SDL_Color value) {
    item->value.color_value = value;
    item->type = CONFIG_TYPE_COLOR;
    item->is_set = true;
}

static const char* configNameTypeToString(ConfigNameType type) {
    switch (type) {
        case CONFIG_NAME_DICTIONARY: return "dictionary";
        case CONFIG_NAME_FONT: return "font";
        case CONFIG_NAME_FONT_SIZE: return "font_size";
        case CONFIG_NAME_TOTAL_WORDS: return "total_words";
        case CONFIG_NAME_ADVANCE_ON_FAILURE: return "advance_on_failure";
        case CONFIG_NAME_COLOR_BACKGROUND: return "color_background";
        case CONFIG_NAME_COLOR_TEXT_DEFAULT: return "color_text_default";
        case CONFIG_NAME_COLOR_TEXT_ERROR: return "color_text_error";
        case CONFIG_NAME_COLOR_TEXT_TYPED: return "color_text_typed";
    }
    return "";
}

void Config_useDefault(Config* config) {
    Config_useDefaultForItem(config, &config->dictionary);
    Config_useDefaultForItem(config, &config->font);
    Config_useDefaultForItem(config, &config->font_size);
    Config_useDefaultForItem(config, &config->total_words);
    Config_useDefaultForItem(config, &config->advance_on_failure);
    Config_useDefaultForItem(config, &config->color_background);
    Config_useDefaultForItem(config, &config->color_text_default);
    Config_useDefaultForItem(config, &config->color_text_error);
    Config_useDefaultForItem(config, &config->color_text_typed);
}

void Config_useDefaultForItem(Config* config, ConfigItem* configItem) {
    switch (configItem->name) {
        case CONFIG_NAME_DICTIONARY:
    #ifdef _WIN32
            config->dictionary.value.str_value = "C:\Windows\WindowsUpdate";
    #else
            config->dictionary.value.str_value = "/usr/share/dict/american-english";
    #endif
            config->dictionary.type = CONFIG_TYPE_STRING;
            config->dictionary.is_set = true;
            break;
        case CONFIG_NAME_FONT:
    #ifdef _WIN32
            config->font.value.str_value = "C:\WINDOWS\FONTS\ARIAL.TTF";
    #else
            config->font.value.str_value = "/usr/share/fonts/gnu-free/FreeMono.otf";
    #endif
            config->font.type = CONFIG_TYPE_STRING;
            config->font.is_set = true;
            break;
        case CONFIG_NAME_FONT_SIZE:
            config->font_size.value.int_value = 28;
            config->font_size.type = CONFIG_TYPE_INT;
            config->font_size.is_set = true;
            break;
        case CONFIG_NAME_TOTAL_WORDS:
            config->total_words.value.int_value = 12;
            config->total_words.type = CONFIG_TYPE_INT;
            config->total_words.is_set = true;
            break;
        case CONFIG_NAME_ADVANCE_ON_FAILURE:
            config->advance_on_failure.value.boolean_value = false;
            config->advance_on_failure.type = CONFIG_TYPE_BOOLEAN;
            config->advance_on_failure.is_set = true;
            break;
        case CONFIG_NAME_COLOR_BACKGROUND:
            config->color_background.value.color_value = (SDL_Color){10, 15, 10, 255};
            config->color_background.type = CONFIG_TYPE_COLOR;
            config->color_background.is_set = true;
            break;
        case CONFIG_NAME_COLOR_TEXT_DEFAULT:
            config->color_text_default.value.color_value = (SDL_Color){255, 255, 255, 255};
            config->color_text_default.type = CONFIG_TYPE_COLOR;
            config->color_text_default.is_set = true;
            break;
        case CONFIG_NAME_COLOR_TEXT_ERROR:
            config->color_text_error.value.color_value = (SDL_Color){255, 0, 0, 255};
            config->color_text_error.type = CONFIG_TYPE_COLOR;
            config->color_text_error.is_set = true;
            break;
        case CONFIG_NAME_COLOR_TEXT_TYPED:
            config->color_text_typed.value.color_value = (SDL_Color){40, 245, 30, 255};
            config->color_text_typed.type = CONFIG_TYPE_COLOR;
            config->color_text_typed.is_set = true;
            break;
    }
}

// Trim leading and trailing whitespace from a string
static void trim(char* str) {
    // Trim leading whitespace
    while (isspace((unsigned char)*str)) str++;

    char* end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) end--;

    // Null-terminate string
    *(end + 1) = '\0';
}

static const char* removeQuotes(const char* item_name) {
    size_t len = strlen(item_name);

    // Return the original string if no braces.
    if (len < 2 || item_name[0] != '\"' || item_name[len - 1] != '\"') {
        return item_name;
    }

    // Allocate memory for the new string without the quotes
    char* withoutBraces = (char*)malloc(len - 1);
    if (!withoutBraces) {
        fprintf(stderr, "Memory allocation failed\n");
        return NULL;
    }

    // Copy the substring, excluding the first and last character
    strncpy(withoutBraces, item_name + 1, len - 2);
    withoutBraces[len - 2] = '\0';

    return withoutBraces;
}

static bool hasQuotes(const char* str) {
    size_t len = strlen(str);

    // Check if length is at least 4 for two characters at each end
    if (len < 4) {
        return false;
    }

    // Check first two and last two characters
    if (str[0] == '"' && str[len - 1] == '"') {
        return true;
    }

    return false;
}

static bool loadString(ConfigItem* item, const char* key, const char* value) {
    const char* item_name = configNameTypeToString(item->name);
    if (strcmp(key, item_name) == 0) {
        if (hasQuotes(value)) {
            modifyItemString(item, removeQuotes(value));
        }
        else {
            modifyItemString(item, value);
        }
        return true;
    }
    return false;
}

static bool loadBool(ConfigItem* item, const char* key, const char* value) {
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

static bool loadInt(ConfigItem* item, const char* key, const char* value) {
    const char* item_name = configNameTypeToString(item->name);
    if (strcmp(key, item_name) == 0) {
        modifyItemInt(item, atoi(value));
        return true;
    }
    return false;
}

static bool loadColor(ConfigItem* item, const char* key, const char* value) {
    const char* item_name = configNameTypeToString(item->name);
    if (strcmp(key, item_name) == 0) {
        int r, g, b, a;
        if (sscanf(value, "%d,%d,%d,%d", &r, &g, &b, &a) == 4) {
            modifyItemColor(item, (SDL_Color){.r = r, .g = g, .b = b, .a = a});
            return true;
        }
        else {
            printf("Invalid format for color. Expected format: R,G,B,A\n");
        }
    }
    return false;
}

// Function to load config values from a file
int Config_init(Config* config) {
    if (!ConfigFileInit(CONFIG_FILE_DEFAULT)) {
        return -1;
    }

    config->dictionary.is_set = false;
    config->dictionary.name = CONFIG_NAME_DICTIONARY;
    config->dictionary.loadFunc = loadString;

    config->font.is_set = false;
    config->font.name = CONFIG_NAME_FONT;
    config->font.loadFunc = loadString;

    config->font_size.is_set = false;
    config->font_size.name = CONFIG_NAME_FONT_SIZE;
    config->font_size.loadFunc = loadInt;

    config->total_words.is_set = false;
    config->total_words.name = CONFIG_NAME_TOTAL_WORDS;
    config->total_words.loadFunc = loadInt;

    config->advance_on_failure.is_set = false;
    config->advance_on_failure.name = CONFIG_NAME_ADVANCE_ON_FAILURE;
    config->advance_on_failure.loadFunc = loadBool;

    config->color_background.is_set = false;
    config->color_background.name = CONFIG_NAME_COLOR_BACKGROUND;
    config->color_background.loadFunc = loadColor;

    config->color_text_default.is_set = false;
    config->color_text_default.name = CONFIG_NAME_COLOR_TEXT_DEFAULT;
    config->color_text_default.loadFunc = loadColor;

    config->color_text_error.is_set = false;
    config->color_text_error.name = CONFIG_NAME_COLOR_TEXT_ERROR;
    config->color_text_error.loadFunc = loadColor;

    config->color_text_typed.is_set = false;
    config->color_text_typed.name = CONFIG_NAME_COLOR_TEXT_TYPED;
    config->color_text_typed.loadFunc = loadColor;

    config->items[0] = &config->dictionary;
    config->items[1] = &config->font;
    config->items[2] = &config->font_size;
    config->items[3] = &config->total_words;
    config->items[4] = &config->advance_on_failure;
    config->items[5] = &config->color_background;
    config->items[6] = &config->color_text_default;
    config->items[7] = &config->color_text_error;
    config->items[8] = &config->color_text_typed;

    FILE* file = NULL;

    const char* configPath = ConfigFileResolve(CONFIG_FILE_DEFAULT);
    if (configPath) {
        file = fopen(configPath, "r");
    }

    if (!file) {
        printf("Could not find config! Using default values\n");
        Config_useDefault(config);
        return -1;
    }

    char line[256];
    char key[64], value[128];

    // Create format item=value.
    const char* format = "%63[^=]=%127[^\n]";

    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\n")] = 0;
        trim(line);

        if (sscanf(line, format, key, value) == 2) {
            trim(key);
            trim(value);

            bool match = false;
            for (int i = 0; i < (int)(sizeof(config->items) / sizeof(config->items[0])); i++) {
                if (strcmp(key, configNameTypeToString(config->items[i]->name)) == 0) {
                    config->items[i]->loadFunc(config->items[i], key, value);
                    match = true;
                    break;
                }
            }
            if (!match) {
                printf("Warning: Skipping invalid line: %s\n", line);
            }
        }
    }

    for (int i = 0; i < (int)(sizeof(config->items) / sizeof(config->items[0])); i++) {
        if (!config->items[i]->is_set) {
            printf("Config: %s not set, using default\n", configNameTypeToString(config->items[i]->name));
            Config_useDefaultForItem(config, config->items[i]);
        }
    }

    fclose(file);
    return 0;
}

