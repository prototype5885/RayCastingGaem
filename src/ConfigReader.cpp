#include "ConfigReader.h"
#include <fstream>
#include <sstream>
#include <string>

using namespace std;

Config ReadConfigFile() {
  Config cfg;
  cfg.fullscreen = false;
  cfg.width = 1920;
  cfg.height = 1080;
  cfg.resolutionPercentage = 100;
  cfg.linearFiltering = false;

#ifdef __EMSCRIPTEN__

  const char *filename = "config.txt";

  ifstream fileExists(filename);
  if (!fileExists.good()) {
    ofstream file(filename);
    file << "fullscreen=false\n";
    file << "width=1920\n";
    file << "height=1080\n";
    file << "resolutionPercentage=100\n";
    file << "linearFiltering=false\n";
  }

  string line;
  ifstream file(filename);

  while (getline(file, line)) {
    if (line.empty() || line[0] == '#')
      continue;

    istringstream iss(line);
    string key, value;

    if (getline(iss, key, '=') && getline(iss, value)) {
      try {
        if (key == "fullscreen") {
          cfg.fullscreen = (value == "true");
        } else if (key == "width") {
          cfg.width = stoi(value);
        } else if (key == "height") {
          cfg.height = stoi(value);
        } else if (key == "resolutionPercentage") {
          cfg.resolutionPercentage = stoi(value);
        } else if (key == "linearFiltering") {
          cfg.linearFiltering = (value == "true");
        }
      } catch (const std::exception &e) {
      }
    }
  }

#endif

  return cfg;
}