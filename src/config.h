#ifndef CONFIG_H
#define CONFIG_H

namespace config {
typedef struct {
  bool fullscreen, linearFiltering, retroResolution;
  int width, height;
  float resolutionPercentage;
} Config;

Config ReadConfigFile();
} // namespace config

#endif