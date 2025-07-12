#include "ConfigReader.h"
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
    const char *filename = "config.txt";
    {
      ifstream fileExists(filename);
      if (!fileExists.good()) {
        ofstream fileWrite(filename);
        if (!fileWrite.good()) {
          throw("Couldn't create config file");
        }

        fileWrite << "fullscreen=false\n";
        fileWrite << "width=1920\n";
        fileWrite << "height=1080\n";
        fileWrite << "resolutionPercentage=100\n";
        fileWrite << "linearFiltering=false\n";
      }
    }
    {
      ifstream fileRead(filename);
      if (!fileRead.good()) {
        throw("Can't read config file");
      }

      string line;
      while (getline(fileRead, line)) {
        if (line.empty() || line[0] == '#')
          continue;

        istringstream iss(line);
        string key, value;

        if (getline(iss, key, '=') && getline(iss, value)) {
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
        }
      }
    }
  } catch (const char *ex) {
    std::cout << ex << endl;
    exit(1);
  }
#endif

  return cfg;
}