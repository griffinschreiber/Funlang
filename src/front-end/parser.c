#include "parser.h"
#include "lexer.h"

struct ast parse(char *src) {
  struct lexer lexer;
  lexer->start = src;
  lexer->current = src;
  lexer->line = 1;
  return parse_expr_bp(&lexer, 0);
}

struct ast parse_bp(struct lexer *lexer, int min_bp) {

}
