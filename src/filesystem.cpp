#include "filesystem.h"

#include <cstring>

namespace filesystem {
File SplitFileNameExtension(char *filename) {
  File file{};

  char *dot = std::strrchr(filename, '.');

  if (!dot || dot == filename)
    return file;

  *dot = '\0';
  file.name = filename;
  file.extension = dot + 1;
  return file;
}
} // namespace filesystem