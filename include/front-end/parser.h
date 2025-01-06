#ifndef PARSER_H_
#define PARSER_H_

struct ast {
  char *value;
  struct ast *children;
};

struct ast parse_expr(char *src);

#endif // PARSER_H_
