#ifndef PARSER_H_
#define PARSER_H_

struct ast {
  struct ast *children;
  // Kinda hacky compiler extension supported by GCC. Basically lets me access the start of the "leftover" memory from the malloc.
  // It obviously doesn't do any bounds checking though, so gotta be careful.
  char value[0];
};

struct ast *parse_expr(char *src);

#endif // PARSER_H_
