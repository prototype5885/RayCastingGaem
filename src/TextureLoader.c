#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

uint32_t *LoadTexture() {
  const int tilemapDimension = 512;
  const int tilemapSize = tilemapDimension * tilemapDimension;

  char filePath[32];

  strcpy(filePath, "textures/textures.bin");

  // texture will be stored in this array
  uint32_t *texture = (uint32_t *)malloc(tilemapSize * sizeof(uint32_t));
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
  uint8_t *buffer = (uint8_t *)malloc(tilemapSize * 3 * sizeof(uint8_t));
  if (buffer == NULL) {
    perror("Error allocating memory for texture loading buffer");
    return NULL;
  }

  // reads the content into the buffer
  size_t elements_read = fread(buffer, sizeof(uint8_t), tilemapSize * 3, file);
  if (elements_read != tilemapSize * 3) {
    perror("Error reading file or wrong texture size");
    fclose(file);
    return NULL;
  }

  // closes the file
  fclose(file);

  // converts the 8 bit array to 32 bit
  for (int p = 0; p < tilemapSize; p++) {
    const int i = p * 3;
    texture[p] = (0 << 24) | (buffer[i] << 16) | (buffer[i + 1] << 8) | buffer[i + 2];
  }
  free(buffer);

  return texture;
}