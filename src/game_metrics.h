#ifndef GAME_METRICS_H
#define GAME_METRICS_H

#include <stddef.h>

typedef struct {
    // Correctly read letters.
    double* accuracy_data;
    size_t accuracy_count;

    // Correctly read words.
    double* speed_data;
    size_t speed_count;
} GameMetrics;

// Initialize a GameMetrics structure
void GameMetrics_init(GameMetrics* metrics);

// Load accuracy data from a file
int GameMetrics_loadAccuracy(GameMetrics* metrics, const char* file_path);

// Load speed data from a file
int GameMetrics_loadSpeed(GameMetrics* metrics, const char* file_path);

// Calculate the average speed in WPM
double GameMetrics_getAverageSpeed(const GameMetrics* metrics);

// Calculate the average accuracy as a percentage
double GameMetrics_getAverageAccuracy(const GameMetrics* metrics);

// Free resources used by GameMetrics
void GameMetrics_free(GameMetrics* metrics);

#endif
