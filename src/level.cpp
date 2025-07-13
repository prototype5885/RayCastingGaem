#include "level.h"
#include "texture.h"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <set>
#include <string>
#include <vector>

using namespace std;
using namespace filesystem;

vector<uint8_t> currentLevel;

int LoadLevel(string name) {
  cout << "Loading level " << name << "..." << endl;

  const path filePath = "assets/levels/" + name + ".txt";
  ifstream file(filePath);
  if (!file.good()) {
    cerr << "Couldn't find level " << filePath << endl;
    return 1;
  }

  currentLevel.clear();

  set<uint8_t> wallTypes;
  uint8_t ch;
  while (file >> ch) {
    if (ch != '\n') {
      uint8_t wallType = static_cast<uint8_t>(ch - '0');
      currentLevel.push_back(wallType);
      if (wallType != 0) {
        wallTypes.insert(wallType);
      }
    }
  }

  cout << "Loaded level " << name << ", bytes: " << currentLevel.size() << endl;
  LoadTextures(wallTypes);

  return 0;
}