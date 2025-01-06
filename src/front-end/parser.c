#include "parser.h"
#include "lexer.h"

void todo() {
  fprintf(stderr, "todo: reached unimplemented feature.\n");
  exit(1);
}

struct ast parse(char *src) {
  if (src[0] == '\0') {
    todo();
  }
  struct lexer lexer = make_lexer(src);
  return parse_bp(&lexer, 0);
}

int get_bp(enum token_type type) {
  switch (type) {

  }
}

struct ast make_binary_op(struct ast lhs, struct token op, struct ast lhs) {

}

struct ast parse_bp(struct lexer *lexer, int min_bp) {
  struct token lhs = lex(lexer);
  if (lhs.type = LEX_EOF) {
    todo();
  }
  for (;;) {
    struct token op = lex(lexer);
    if (op.type = LEX_EOF) {
      break;
    }
    int op_bp = get_bp(op.type);
    if (op_bp < min_bp) {
      todo();
    }
    struct ast rhs = parse_bp(lexer, op_bp);
    lhs = make_binary_op(lhs, op, rhs);
  }
  return lhs;
}
