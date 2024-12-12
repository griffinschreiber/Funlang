#include <stdio.h>
#include <stdlib.h>

#include "utils.h"
#include "front-end/lexer.h"

int main(int argc, char *argv[]) {
  if (argc < 2) {
    printf("Usage: ./main file\n");
    return 0;
  }
  char *src = read_file(argv[1]);

  struct lexer lexer;
  lexer.src = src;
  lexer.start = src;
  lexer.len = 0;
  lexer.line = 1;
  lexer.scratchpad[LEX_SCRATCHPAD_SIZE - 1] = '\0';

  printf("Debug: lexing first token.\n");
  while (lex(&lexer)->type != LEX_EOF) {
    printf("Debug: lexing another token.\n");
  }

  free(src);
  return 0;
}
