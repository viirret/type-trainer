#ifndef CONFIG_H
#define CONFIG_H

#include <SDL2/SDL_pixels.h>

#include <stdbool.h>

typedef enum {
    CONFIG_NAME_DICTIONARY,
    CONFIG_NAME_FONT,
    CONFIG_NAME_FONT_SIZE,
    CONFIG_NAME_TOTAL_WORDS,
    CONFIG_NAME_ADVANCE_ON_FAILURE,
    CONFIG_NAME_COLOR_BACKGROUND,
    CONFIG_NAME_COLOR_TEXT_DEFAULT,
    CONFIG_NAME_COLOR_TEXT_ERROR,
    CONFIG_NAME_COLOR_TEXT_TYPED,
} ConfigNameType;

typedef enum {
    CONFIG_TYPE_STRING,
    CONFIG_TYPE_INT,
    CONFIG_TYPE_COLOR,
    CONFIG_TYPE_BOOLEAN
} ConfigValueType;

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
    ConfigItem dictionary;
    ConfigItem font;
    ConfigItem font_size;
    ConfigItem total_words;
    ConfigItem advance_on_failure;
    ConfigItem color_background;
    ConfigItem color_text_default;
    ConfigItem color_text_error;
    ConfigItem color_text_typed;
} Config;

int Config_load(Config* config);
void Config_useDefault(Config* config);
void Config_useDefaultForItem(Config* config, ConfigItem* configItem);

#endif
