#include <cstdint>
#include <fstream>
#include <iostream>
#include <vector>

using namespace std;

uint32_t *LoadTexture() {
  const int tilemapDimension = 512;
  const int tilemapSize = tilemapDimension * tilemapDimension;

  const string filePath = "textures/textures.bin";

  // texture will be stored in this array on the heap
  uint32_t *texture = new uint32_t[tilemapSize];

  // opens the texture binary file for reading
  ifstream file(filePath, ios::binary);
  if (!file) {
    cerr << "Error opening file: " << filePath << endl;
    return NULL;
  }

  // creates vector reads the content into
  vector<uint8_t> buffer((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());

  // closes the file
  file.close();

  // convers the 8 bit array to 32 bit
  for (int p = 0; p < tilemapSize; p++) {
    const int i = p * 3;
    texture[p] = (0 << 24) | (buffer[i + 0] << 16) | (buffer[i + 1] << 8) | buffer[i + 2];
  }

  return texture;
}