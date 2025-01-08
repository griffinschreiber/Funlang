#include "front-end/parser.h"

#include "front-end/lexer.h"
#include "utils/arena.h"

void todo() {
  fprintf(stderr, "todo: reached unimplemented feature.\n");
  exit(1);
}

struct ast *parse(char *src) {
  if (src[0] == '\0') {
    todo();
  }
  struct arena = make_arena();
  struct lexer lexer = make_lexer(src);
  return parse_bp(&arena, &lexer, 0);
}

int get_lbp(enum token_type type) {
  switch (type) {

  }
}

int get_rbp(enum token_type type) {
  switch (type) {

  }
}

struct ast *make_atom(struct arena *arena, struct token value) {
  struct ast atom = (struct ast *)arena_alloc(arena, sizeof(struct ast));
  atom.value = value;
  atom.children = NULL;
  return &atom;
}

struct ast *make_binary_op(struct ast lhs, struct token op, struct ast rhs) {
  struct ast op_node = (struct ast *)arena_alloc(arena, sizeof(struct ast));
  op_node.value = op;
  op_node.children = {lhs, rhs};
  return &op_node;
}

struct ast *make_unary_op(struct token op, struct ast rhs) {
  struct ast op_node = (struct ast *)arena_alloc(arena, sizeof(struct ast));
  op_node.value = op;
  op_node.children = rhs;
  return &op_node;
}

struct ast *parse_bp(struct arena *arena, struct lexer *lexer, int min_bp) {
  struct ast *lhs = make_atom(arena, lex(lexer));
  if (lhs.value.type = LEX_EOF) {
    return lhs;
  }
  int lhs_rbp = get_rbp(lhs.value.type);
  if (lhs_rbp) {
    struct ast rhs = parse_bp(lexer, lhs_rbp);
    return make_unary_op(arena, lhs.value, rhs);
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
    lhs = make_binary_op(arena, lhs, op, rhs);
  }
  return lhs;
}
