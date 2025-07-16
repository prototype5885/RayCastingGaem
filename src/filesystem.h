//
// Created by ProToType on 2025. 07. 16..
//

#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#define MAX_FILEPATH_LENGTH 260

namespace filesystem {

struct File {
  char *name;
  char *extension;
};

File SplitFileNameExtension(char *filename);
} // namespace filesystem

#endif // FILESYSTEM_H
