#include "config_file.h"

#include <sys/stat.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

// Helper function to create directories recursively
bool create_directory(const char* path) {
    char temp[512];
    snprintf(temp, sizeof(temp), "%s", path);

    for (char* p = temp + 1; *p; p++) {
        if (*p == '/') {
            *p = '\0'; // Temporarily terminate string
            if (mkdir(temp, 0755) == -1 && errno != EEXIST) {
                perror("Failed to create directory");
                return false;
            }
            *p = '/'; // Restore the slash
        }
    }
    // Create the final directory in case it's not covered in the loop
    if (mkdir(temp, 0755) == -1 && errno != EEXIST) {
        perror("Failed to create directory");
        return false;
    }

    return true;
}

bool createConfigFiles() {
    const char* home = getenv("HOME");
    if (!home) {
        fprintf(stderr, "Error: HOME environment variable is not set.\n");
        return false;
    }

    // Resolve paths
    const char* config_file = ConfigFileResolve(CONFIG_FILE_DEFAULT);
    const char* speed_file = ConfigFileResolve(CONFIG_DATA_FILE_SPEED);
    const char* accuracy_file = ConfigFileResolve(CONFIG_DATA_FILE_ACCURACY);

    if (!config_file || !speed_file || !accuracy_file) {
        fprintf(stderr, "Error: Config file resolution failed.\n");
        free((void*)config_file);
        free((void*)speed_file);
        free((void*)accuracy_file);
        return false;
    }

    // Extract directories from file paths
    char config_dir[512], speed_dir[512], accuracy_dir[512];

    strncpy(config_dir, config_file, sizeof(config_dir));
    strncpy(speed_dir, speed_file, sizeof(speed_dir));
    strncpy(accuracy_dir, accuracy_file, sizeof(accuracy_dir));

    char* last_slash;

    // Extract directory path for config
    last_slash = strrchr(config_dir, '/');
    if (last_slash) *last_slash = '\0';
    else strcpy(config_dir, "."); // If no slash, default to current directory

    // Extract directory path for speed
    last_slash = strrchr(speed_dir, '/');
    if (last_slash) *last_slash = '\0';
    else strcpy(speed_dir, ".");

    // Extract directory path for accuracy
    last_slash = strrchr(accuracy_dir, '/');
    if (last_slash) *last_slash = '\0';
    else strcpy(accuracy_dir, ".");

    // Ensure directories exist
    if (!create_directory(config_dir)) {
        fprintf(stderr, "Failed to create config directory: %s\n", config_dir);
        free((void*)config_file);
        free((void*)speed_file);
        free((void*)accuracy_file);
        return false;
    }
    if (!create_directory(speed_dir)) {
        fprintf(stderr, "Failed to create speed directory: %s\n", speed_dir);
        free((void*)config_file);
        free((void*)speed_file);
        free((void*)accuracy_file);
        return false;
    }
    if (!create_directory(accuracy_dir)) {
        fprintf(stderr, "Failed to create accuracy directory: %s\n", accuracy_dir);
        free((void*)config_file);
        free((void*)speed_file);
        free((void*)accuracy_file);
        return false;
    }

    // Create or open each file
    FILE* c_file = fopen(config_file, "a");
    if (!c_file) {
        perror("Failed to create config file");
        free((void*)config_file);
        free((void*)speed_file);
        free((void*)accuracy_file);
        return false;
    }
    fclose(c_file);

    FILE* s_file = fopen(speed_file, "a");
    if (!s_file) {
        perror("Failed to create speed file");
        free((void*)config_file);
        free((void*)speed_file);
        free((void*)accuracy_file);
        return false;
    }
    fclose(s_file);

    FILE* a_file = fopen(accuracy_file, "a");
    if (!a_file) {
        perror("Failed to create accuracy file");
        free((void*)config_file);
        free((void*)speed_file);
        free((void*)accuracy_file);
        return false;
    }
    fclose(a_file);
    free((void*)config_file);
    free((void*)speed_file);
    free((void*)accuracy_file);
    return true;
}


bool ConfigFileExists(const char* file_name) {
    const char* file_path = ConfigFileResolve(file_name);
    FILE* file = fopen(file_path, "r");
    free((void*)file_path);
    return file;
}

bool ConfigFileEmpty(const char* file_name) {
    const char* resolved_file = ConfigFileResolve(file_name);
    FILE* file = fopen(resolved_file, "r");
    free((void*)resolved_file);
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
    const char* resolved_file = ConfigFileResolve(file_name);
    FILE* file = fopen(resolved_file, "r");

    if (!file) {
        // If the file doesn't exist, try to create it in write mode
        file = fopen(resolved_file, "w");
        if (!file) {
            printf("Failed to create new config file %s", resolved_file);
            free((void*)resolved_file);
            return false;
        }

        // Close the newly created file
        fclose(file);

        // Check if the file was created successfully by re-opening it in read mode
        file = fopen(resolved_file, "r");
        if (file) {
            free((void*)resolved_file);
            fclose(file);
            return true;  // File created successfully
        }
        else {
            free((void*)resolved_file);
            perror("Failed to verify new config file");
            return false;
        }
    }
    else {
        free((void*)resolved_file);
        return true;
    }
}

bool ConfigFileWriteInt(const char* file_name, int number) {
    if (ConfigFileExists(file_name)) {
        const char* resolved_file = ConfigFileResolve(file_name);
        FILE* file = fopen(resolved_file, "w");
        free((void*)resolved_file);
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

    char* config_path = (char*)malloc(512);
    if (!config_path) {
        fprintf(stderr, "Error: Memory allocation failed.\n");
        return NULL;
    }

    if (strcmp(config_file, CONFIG_FILE_DEFAULT) == 0) {
        if (xdg_config_home && strlen(xdg_config_home) > 0) {
            snprintf(config_path, 512, "%s/%s", xdg_config_home, config_file);
        } else if (home) {
            snprintf(config_path, 512, "%s/.config/%s", home, config_file);
        } else {
            fprintf(stderr, "Error: Neither XDG_CONFIG_HOME nor HOME is set.\n");
            return NULL;
        }
    }
    else if (strcmp(config_file, CONFIG_DATA_FILE_SPEED) == 0) {
        if (xdg_data_home && strlen(xdg_data_home) > 0) {
            snprintf(config_path, 512, "%s/%s", xdg_data_home, config_file);
        } else {
            snprintf(config_path, 512, "%s/.local/share/%s", home, config_file);
        }
    }
    else if (strcmp(config_file, CONFIG_DATA_FILE_ACCURACY) == 0) {
        if (xdg_data_home && strlen(xdg_data_home) > 0) {
            snprintf(config_path, 512, "%s/%s", xdg_data_home, config_file);
        } else {
            snprintf(config_path, 512, "%s/.local/share/%s", home, config_file);
        }
    }
    else {
        fprintf(stderr, "Error: Unknown config file key.\n");
        free(config_path);
        return NULL;
    }

    return config_path;
}
