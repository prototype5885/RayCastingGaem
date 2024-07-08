#include <cstddef>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <string>

using std::cerr;
using std::cout;
using std::endl;
using std::ifstream;
using std::ios;
using std::string;

#if defined(_WIN32) || defined(_WIN64)
const string PATH_SEPARATOR = "\\";
#elif defined(__linux__)
const string PATH_SEPARATOR = "/";
#else
const string PATH_SEPARATOR = "/"; // for non windows and linux
#endif

uint32_t *LoadTexture(string name, int textureWidth, int textureHeight) {
  // const int textureWidth = width;
  const int tSize = textureWidth * textureHeight;

  const string filePath = "textures" + PATH_SEPARATOR + name + ".bin";

  // texture will be stored in this array on the heap
  uint32_t *texture = new uint32_t[tSize];

  // opens the texture binary file for reading
  cout << "opening file..." << endl;
  ifstream file(filePath, ios::binary);
  if (!file) {
    cerr << "Error opening file: " << filePath << endl;
    return NULL;
  }

  // creates buffer and reads into it
  uint8_t *buffer = new uint8_t[tSize * 3];
  file.read(reinterpret_cast<char *>(buffer), tSize);

  // read the content into a vector
  // std::istreambuf_iterator<char> start(file);
  // std::istreambuf_iterator<char> end;
  // std::vector<uint8_t> buffer(start, end);

  // closes the file
  file.close();

  // convers the 8 bit array to 32 bit
  for (int p = 0; p < tSize; p++) {
    const int i = p * 3;
    texture[p] = (0 << 24) | (buffer[i + 0] << 16) | (buffer[i + 1] << 8) | buffer[i + 2];
  }

  delete[] buffer;

  return texture;
}