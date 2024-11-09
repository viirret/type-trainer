#ifndef CONFIG_FILE_H
#define CONFIG_FILE_H

#include <stdbool.h>

#define CONFIG_FILE_DEFAULT  "/.config/type-trainer/config.txt"
#define CONFIG_DATA_FILE_ACCURACY "/.config/type-trainer/accuracy"
#define CONFIG_DATA_FILE_SPEED    "/.config/type-trainer/speed"

bool ConfigFileInit(const char* file_name);
bool ConfigFileExists(const char* file_name);
bool ConfigFileEmpty(const char* file_name);
bool ConfigFileWriteInt(const char* file_name, int number);
const char* ConfigFileResolve(const char* file_name);

#endif
