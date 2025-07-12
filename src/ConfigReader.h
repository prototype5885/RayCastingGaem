#ifndef CONFIG_READER_H
#define CONFIG_READER_H

typedef struct {
  char fullscreen, linearFiltering;
  int width, height, resolutionPercentage;
} Config;

Config ReadConfigFile();

#endif