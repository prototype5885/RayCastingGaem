#ifndef CONFIG_READER_H
#define CONFIG_READER_H

using namespace std;

typedef struct {
  bool fullscreen, linearFiltering;
  int width, height, resolutionPercentage;
} Config;

Config ReadConfigFile();

#endif