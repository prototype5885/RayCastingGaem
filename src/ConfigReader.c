#include "ConfigReader.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

Config ReadConfigFile() {
  Config cfg;
  cfg.fullscreen = 0;
  cfg.width = 1920;
  cfg.height = 1080;
  cfg.resolutionPercentage = 100;
  cfg.linearFiltering = 0;

#ifndef __EMSCRIPTEN__

  const char *filename = "config.txt";

  FILE *file = fopen(filename, "r");
  if (file == NULL) {
    file = fopen(filename, "w");

    fprintf(file, "fullscreen=false\n");
    fprintf(file, "width=1280\n");
    fprintf(file, "height=720\n");
    fprintf(file, "resolutionPercentage=100\n");
    fprintf(file, "linearFiltering=false\n");
    fclose(file);
  }

  file = fopen(filename, "r");
  if (file == NULL) {
    return cfg;
  }

  char line[32];

  while (fgets(line, sizeof(line), file)) {
    if (line[0] == '\n' || line[0] == '#')
      continue;
    line[strcspn(line, "\n")] = 0;

    char *equals = strchr(line, '=');
    if (!equals)
      continue;

    *equals = '\0';
    char *key = line;
    char *value = equals + 1;

    if (!strcmp(key, "fullscreen")) {
      cfg.fullscreen = (!strcmp(value, "true") || !strcmp(value, "1"));
    } else if (!strcmp(key, "width")) {
      cfg.width = atoi(value);
    } else if (!strcmp(key, "height")) {
      cfg.height = atoi(value);
    } else if (!strcmp(key, "resolutionPercentage")) {
      cfg.resolutionPercentage = atoi(value);
    } else if (!strcmp(key, "linearFiltering")) {
      cfg.linearFiltering = (!strcmp(value, "true") || !strcmp(value, "1"));
    }
  }
  fclose(file);

#endif

  return cfg;
}