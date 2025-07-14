#include "config.h"

#include <fstream>
#include <iostream>
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

#ifndef __EMSCRIPTEN__
  try {
    auto filename = "config.txt";
    {
      ifstream fileExists(filename);
      if (!fileExists.good()) {
        ofstream fileWrite(filename);
        if (!fileWrite.good()) {
          throw runtime_error("Can't write config file");
        }

        fileWrite << "fullscreen=false\n";
        fileWrite << "width=1920\n";
        fileWrite << "height=1080\n";
        fileWrite << "retroResolution=false\n";
        fileWrite << "resolutionPercentage=100\n";
        fileWrite << "linearFiltering=false\n";
      }
    }
    {
      ifstream fileRead(filename);
      if (!fileRead.good()) {
        throw runtime_error("Can't read config file");
      }

      string line;
      while (getline(fileRead, line)) {
        if (line.empty() || line[0] == '#')
          continue;

        istringstream iss(line);
        string key, value;

        if (getline(iss, key, '=') && getline(iss, value)) {
          if (key == "fullscreen") {
            cfg.fullscreen = value == "true";
          } else if (key == "width") {
            cfg.width = stoi(value);
          } else if (key == "height") {
            cfg.height = stoi(value);
          } else if (key == "retroResolution") {
            cfg.retroResolution = value == "true";
          } else if (key == "resolutionPercentage") {
            cfg.resolutionPercentage = stof(value);
          } else if (key == "linearFiltering") {
            cfg.linearFiltering = value == "true";
          }
        }
      }
    }
  } catch (const std::runtime_error& e) {
    cout << e.what() << endl;
    exit(1);
  }
#endif

  return cfg;
}