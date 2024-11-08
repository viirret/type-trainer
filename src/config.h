#ifndef CONFIG_H
#define CONFIG_H

#include <SDL2/SDL_pixels.h>

#include <stdbool.h>

typedef enum {
    CONFIG_TYPE_STRING,
    CONFIG_TYPE_INT,
    CONFIG_TYPE_COLOR,
    CONFIG_TYPE_BOOLEAN
} ConfigValueType;

typedef enum {
    CONFIG_NAME_FONT,
    CONFIG_NAME_PATH_SIZE,
    CONFIG_NAME_TOTAL_WORDS,
    CONFIG_NAME_BACKGROUND_COLOR,
    CONFIG_NAME_ADVANCE_ON_FAILURE
} ConfigNameType;

typedef union {
    char* str_value;
    int int_value;
    SDL_Color color_value;
    bool boolean_value;
} ConfigValue;

typedef struct {
    ConfigNameType name;
    ConfigValue value;
    ConfigValueType type;
    bool is_set;
} ConfigItem;

typedef struct {
    ConfigItem font;
    ConfigItem path_size;
    ConfigItem total_words;
    ConfigItem background_color;
    ConfigItem advance_on_failure;
} Config;

int Config_load(Config* config);
void Config_destroy(Config* config);
void Config_useDefault(Config* config);
void Config_useDefaultForItem(Config* config, ConfigItem configItem);

#endif
