#include "game_metrics.h"

#include <stdio.h>
#include <stdlib.h>

// Initialize the GameMetrics structure with default values
void GameMetrics_init(GameMetrics* metrics) {
    metrics->accuracy_data = NULL;
    metrics->accuracy_count = 0;
    metrics->speed_data = NULL;
    metrics->speed_count = 0;
}

// Load accuracy data from a file, storing values line-by-line into accuracy_data
int GameMetrics_loadAccuracy(GameMetrics* metrics, const char* file_path) {
    FILE* file = fopen(file_path, "r");
    if (!file) {
        perror("Failed to open accuracy file");
        return -1;
    }

    double value;
    size_t count = 0;

    while (fscanf(file, "%lf", &value) == 1) {
        metrics->accuracy_data = realloc(metrics->accuracy_data, sizeof(double) * (count + 1));
        if (!metrics->accuracy_data) {
            perror("Memory allocation failed");
            fclose(file);
            return -1;
        }
        metrics->accuracy_data[count++] = value;
    }

    metrics->accuracy_count = count;
    fclose(file);
    return 0;
}

// Load speed data from a file, storing values line-by-line into speed_data
int GameMetrics_loadSpeed(GameMetrics* metrics, const char* file_path) {
    FILE* file = fopen(file_path, "r");
    if (!file) {
        perror("Failed to open speed file");
        return -1;
    }

    double value;
    size_t count = 0;

    while (fscanf(file, "%lf", &value) == 1) {
        metrics->speed_data = realloc(metrics->speed_data, sizeof(double) * (count + 1));
        if (!metrics->speed_data) {
            perror("Memory allocation failed");
            fclose(file);
            return -1;
        }
        metrics->speed_data[count++] = value;
    }

    metrics->speed_count = count;
    fclose(file);
    return 0;
}

// Calculate average speed in WPM
double GameMetrics_getAverageSpeed(const GameMetrics* metrics) {
    if (metrics->speed_count == 0) {
        return 0.0;
    }

    double total_speed = 0.0;
    for (size_t i = 0; i < metrics->speed_count; i++) {
        total_speed += metrics->speed_data[i];
    }

    return total_speed / metrics->speed_count;
}

// Calculate average accuracy as a percentage
double GameMetrics_getAverageAccuracy(const GameMetrics* metrics) {
    if (metrics->accuracy_count == 0) {
        return 0.0;
    }

    double total_accuracy = 0.0;
    for (size_t i = 0; i < metrics->accuracy_count; i++) {
        total_accuracy += metrics->accuracy_data[i];
    }

    return total_accuracy / metrics->accuracy_count;
}

// Free allocated memory used in GameMetrics
void GameMetrics_free(GameMetrics* metrics) {
    free(metrics->accuracy_data);
    free(metrics->speed_data);
    metrics->accuracy_data = NULL;
    metrics->speed_data = NULL;
    metrics->accuracy_count = 0;
    metrics->speed_count = 0;
}

