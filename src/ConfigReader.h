#ifndef CONFIG_READER_H
#define CONFIG_READER_H

typedef struct {
  bool fullscreen, linearFiltering;
  int width, height;
  float resolutionPercentage;
} Config;

Config ReadConfigFile();

#endif