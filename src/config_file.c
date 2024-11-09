#include "config_file.h"

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

bool ConfigFileExists(const char* file_name) {
    const char* file_path = ConfigFileResolve(file_name);
    FILE* file = fopen(file_path, "r");
    return file;
}

bool ConfigFileEmpty(const char* file_name) {
    FILE* file = fopen(ConfigFileResolve(file_name), "r");
    if (!file) {
        perror("Failed to open file");
        return false;
    }

    // Check if we can read a single character to confirm file is not empty
    int ch = fgetc(file);
    fclose(file);

    if (ch == EOF) {
        return true;  // File is empty
    }
    return false; // File has content
}

bool ConfigFileInit(const char* file_name) {
    const char* file_path = ConfigFileResolve(file_name);

    FILE* file = fopen(file_path, "r");

    if (!file){
        // If the file doesn't exist, try to create it in write mode
        file = fopen(file_path, "w");
        if (!file) {
            perror("Failed to create new config file");
            return false;
        }

        // Close the newly created file
        fclose(file);

        // Check if the file was created successfully by re-opening it in read mode
        file = fopen(file_path, "r");
        if (file) {
            fclose(file);
            return true;  // File created successfully
        }
        else {
            perror("Failed to verify new config file");
            return false;
        }
    }
    else {
        return true;
    }
}

bool ConfigFileWriteInt(const char* file_name, int number) {
    if (ConfigFileExists(file_name)) {
        FILE* file = fopen(ConfigFileResolve(file_name), "w");
        if (!file) {
            perror("Failed to open file for writing");
            return false;
        }

        // Write the number to the file.
        if (fprintf(file, "%d", number) < 0) {
            perror("Failed to write integer to the file");
            fclose(file);
            return false;
        }
        fclose(file);
        return true;
    }
    else {
        return false;
    }
}

const char* ConfigFileResolve(const char* config_file) {
    const char* home = getenv("HOME");
    const char* xdg_config_home = getenv("XDG_CONFIG_HOME");
    const char* xdg_data_home = getenv("XDG_DATA_HOME");

    static char config_path[512];

    if (strcmp(config_file, CONFIG_FILE_DEFAULT) == 0) {
        if (xdg_config_home && strlen(xdg_config_home) > 0) {
            // Use XDG_CONFIG_HOME if it is set
            snprintf(config_path, sizeof(config_path), "%s%s", xdg_config_home, config_file);
        }
        else if (home) {
            // Fallback to HOME/.config if XDG_CONFIG_HOME is not set
            snprintf(config_path, sizeof(config_path), "%s%s", home, config_file);
        } else {
            fprintf(stderr, "Error: Neither XDG_CONFIG_HOME nor HOME is set.\n");
            return NULL;
        }
    }
    else if (strcmp(config_file, CONFIG_DATA_FILE_SPEED) == 0 || strcmp(config_file, CONFIG_DATA_FILE_ACCURACY) == 0) {
        if (xdg_config_home && strlen(xdg_config_home) > 0) {
            snprintf(config_path, sizeof(config_path), "%s%s", xdg_data_home, config_file);
        }
        else if (home) {
            snprintf(config_path, sizeof(config_path), "%s%s", home, config_file);
        }
        else {
            fprintf(stderr, "Error: Neither XDG_CONFIG_HOME nor HOME is set.\n");
            return NULL;
        }
    }
    else {
        printf("No valid config file path");
    }

    return config_path;
}
