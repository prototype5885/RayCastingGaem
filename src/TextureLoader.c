#include <bits/stdint-uintn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

uint32_t *LoadTexture(int textureIndex) {

  char *fileName;

  if (textureIndex == 0) {
    fileName = "wall1";
  } else if (textureIndex == 1) {
    fileName = "wall2";
  } else if (textureIndex == 2) {
    fileName = "text1";
  }

  char filePath[32];

  strcpy(filePath, "textures/");
  strcat(filePath, fileName);
  strcat(filePath, ".bin");

  // texture will be stored in this array
  uint32_t *texture = (uint32_t *)malloc(64 * 64 * sizeof(uint32_t));
  if (texture == NULL) {
    perror("Error allocating memory for texture");
    return NULL;
  }

  // opens the texture binary file for reading
  FILE *file = fopen(filePath, "rb");
  if (file == NULL) {
    perror("Error opening file");
    return NULL;
  }

  // creates buffer to store the content
  uint8_t *buffer = (uint8_t *)malloc(64 * 64 * 3 * sizeof(uint8_t));
  if (buffer == NULL) {
    perror("Error allocating memory for texture loading buffer");
    return NULL;
  }

  // reads the content into the buffer
  size_t elements_read = fread(buffer, sizeof(uint8_t), 64 * 64 * 3, file);
  if (elements_read != 64 * 64 * 3) {
    perror("Error reading file or wrong texture size");
    fclose(file);
    return NULL;
  }

  // closes the file
  fclose(file);

  // convers the 8 bit array to 32 bit
  for (int p = 0; p < 64 * 64; p++) {
    const int i = p * 3;
    texture[p] = (0 << 24) | (buffer[i + 0] << 16) | (buffer[i + 1] << 8) | buffer[i + 2];
  }

  return texture;
}