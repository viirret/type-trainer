#include "config_file.h"

#include <stdio.h>
#include <stdlib.h>

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

const char* ConfigFileResolve(const char* name) {
    static char file_name[256];
    const char* home = getenv("HOME");
    if (home) {
        snprintf(file_name, sizeof(file_name), "%s%s", home, name);
    }
    return file_name;
}
