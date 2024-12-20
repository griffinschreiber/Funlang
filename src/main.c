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

  for (;;) {
    printf("Debug: lexing a token.\n");
    struct token token = lex(&lexer);
    printf("Debug: token.type is %i\n", token.type);
    if (token.type == LEX_EOF) {
      printf("Debug: LEX_EOF emitted.\n");
      break;
    }
    printf("Token start: \"%s\"\n", token.start);
  }

  free(src);
  return 0;
}
