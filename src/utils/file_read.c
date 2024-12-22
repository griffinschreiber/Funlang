#include "utils/file_read.h"

#include <stdio.h>
#include <stdlib.h>


char *read_file(const char *path) {
  FILE *file = fopen(path, "r");
  if (!file) {
    fprintf(stderr, "Could not open file at %s\n", path);
    exit(1);
  }

  fseek(file, 0, SEEK_END);
  ulong size = ftell(file);
  rewind(file);

  char *buffer = malloc(size + 1);
  if (!buffer) {
    fprintf(stderr, "read_file: Could not allocate memory for buffer.");
    fclose(file);
    exit(1);
  }

  if (fread(buffer, 1, size, file) != size) {
    fprintf(stderr, "read_file: Could not read all of file at \"%s\".", path);
    free(buffer);
    fclose(file);
    exit(1);
  }

  buffer[size + 1] = '\0';
  fclose(file);
  return buffer;
}
