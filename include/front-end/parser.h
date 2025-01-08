#ifndef PARSER_H_
#define PARSER_H_

#include "front-end/lexer.h"

struct ast {
  struct token value;
  struct ast *children;
};

struct ast *parse_expr(char *src);

#endif // PARSER_H_
