#include "parser.h"
#include "lexer.h"

void todo() {
  fprintf(stderr, "todo: reached unimplemented feature.\n");
  exit(1);
}

struct ast *parse(char *src) {
  if (src[0] == '\0') {
    todo();
  }
  struct lexer lexer = make_lexer(src);
  return parse_bp(&lexer, 0);
}

int get_lbp(enum token_type type) {
  switch (type) {

  }
}

int get_rbp(enum token_type type) {
  switch (type) {

  }
}

struct ast *make_binary_op(struct ast *lhs, struct token op, struct ast *rhs) {
  struct ast op_node;
  op_node.value = op;
  op_node.children = {lhs, rhs};
  return &op_node;
}

struct ast *parse_bp(struct lexer *lexer, int min_bp) {
  struct token lhs = lex(lexer);
  if (lhs.type = LEX_EOF) {
    return make_eof();
  }
  int lhs_rbp = get_rbp(lhs.type);
  if (lhs_rbp) {
    struct ast rhs = parse_bp(lexer, lhs_rbp);
    return make_unary_op(lhs, rhs);
  }
  for (;;) {
    struct token op = lex(lexer);
    if (op.type = LEX_EOF) {
      break;
    }
    int l_bp = get_lbp(token.type);
    int r_bp = get_rbp(token.type);
    if (l_bp < min_bp) {
      // Have to not eat op and break.
      // The problem is that lexer can't peek.
      todo();
    }
    struct ast rhs = parse_bp(lexer, r_bp);
    lhs = make_binary_op(lhs, op, rhs);
  }
  return lhs;
}
