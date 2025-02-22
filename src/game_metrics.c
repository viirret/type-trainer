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

// Load data from a file into a dynamically allocated array.
static int GameMetrics_loadData(const char* file_path, double** data, size_t* count) {
    FILE* file = fopen(file_path, "r");
    if (!file) {
        perror("Failed to open file");
        return -1;
    }

    double value;
    size_t temp_count = 0;
    double* temp_data = NULL;

    while (fscanf(file, "%lf", &value) == 1) {
        double* new_data = realloc(temp_data, sizeof(double) * (temp_count + 1));
        if (!new_data) {
            perror("Memory allocation failed");
            free(temp_data);
            fclose(file);
            return -1;
        }
        temp_data = new_data;
        temp_data[temp_count++] = value;
    }

    fclose(file);
    *data = temp_data;
    *count = temp_count;
    return 0;
}

// Load accuracy data
int GameMetrics_loadAccuracy(GameMetrics* metrics, const char* file_path) {
    return GameMetrics_loadData(file_path, &metrics->accuracy_data, &metrics->accuracy_count);
}

// Load speed data
int GameMetrics_loadSpeed(GameMetrics* metrics, const char* file_path) {
    return GameMetrics_loadData(file_path, &metrics->speed_data, &metrics->speed_count);
}

// Calculate the average of an array
static double GameMetrics_getAverage(const double* data, size_t count) {
    if (count == 0) {
        return 0.0;
    }

    double total = 0.0;
    for (size_t i = 0; i < count; i++) {
        total += data[i];
    }

    return total / count;
}

// Get average speed
double GameMetrics_getAverageSpeed(const GameMetrics* metrics) {
    return GameMetrics_getAverage(metrics->speed_data, metrics->speed_count);
}

// Get average accuracy
double GameMetrics_getAverageAccuracy(const GameMetrics* metrics) {
    return GameMetrics_getAverage(metrics->accuracy_data, metrics->accuracy_count);
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

