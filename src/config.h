#ifndef CONFIG_H
#define CONFIG_H

namespace config {
class Config {
public:
  bool fullscreen, linearFiltering, retroResolution, multiThreaded;
  int width, height;
  float resolutionPercentage;
};

Config ReadConfigFile();
} // namespace config

#endif