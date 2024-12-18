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
  lexer.start = src;
  lexer.current = src;
  lexer.line = 1;

  struct token token;

  printf("Debug: lexing first token.\n");
  while ((token = lex(&lexer)).type != LEX_EOF) {
    printf("Debug: lexing another token.\n");
    printf("Token start: \"%s\"\n", token.start);
  }

  free(src);
  return 0;
}
