#include <stdio.h>
#include <stdlib.h>

#include "utils.h"

int main(int argc, char *argv[]) {
  if (argc < 2) {
    printf("Usage: ./main file\n");
    return 0;
  }
  char *src = read_file(argv[1]);

  free(src);
  return 0;
}
