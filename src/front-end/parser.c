#include "front-end/parser.h"

#include "front-end/lexer.h"

void die(const char *msg) {
  fprintf(stderr, "parser error: %s\n", msg);
}

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
  return parse_bp(&lexer, 0);
}

// literal/identifier
// ,
// arithmetic
// comparison
// assignment
// unary
// array index
int get_lbp(enum token_type type) {
  switch (type) {
  case LEX_EOF:
  case LEX_SEMICOLON:
  case LEX_IDENTIFIER:
  case LEX_NUM_LITERAL:
  case LEX_HEX_LITERAL:
  case LEX_BINARY_LITERAL:
  case LEX_OCTAL_LITERAL:
  case LEX_CHAR_LITERAL:
  case LEX_STR_LITERAL:
    return 0;
  case LEX_COMMA:
    return 1;
  case LEX_PLUS:
  case LEX_MINUS:
    return 2;
  case LEX_STAR:
  case LEX_SLASH:
    return 3;
  case LEX_BITWISE_AND:
  case LEX_BITWISE_OR:
  case LEX_BITWISE_XOR:
    return 4;
  }
}

int get_rbp(enum token_type type) {
  switch (type) {
    case LEX_EOF:
    case LEX_SEMICOLON:
    case LEX_IDENTIFIER:
    case LEX_NUM_LITERAL:
    case LEX_HEX_LITERAL:
    case LEX_BINARY_LITERAL:
    case LEX_OCTAL_LITERAL:
    case LEX_CHAR_LITERAL:
    case LEX_STR_LITERAL:
      return 0;
    case LEX_PLUS:
    case LEX_MINUS:
      return 3;
    case LEX_STAR:
    case LEX_SLASH:
      return 4;
    case LEX_BITWISE_AND:
    case LEX_BITWISE_OR:
    case LEX_BITWISE_XOR:
      return 5;
  }
}

struct ast *make_atom(struct arena *arena, struct token value) {
  struct ast atom = (struct ast *)arena_alloc(arena, sizeof(struct ast));
  atom.value = value;
  atom.children = NULL;
  return &atom;
}

struct ast *make_binary_op(struct arena *arena, struct ast lhs, struct token op, struct ast rhs) {
  struct ast op_node = (struct ast *)arena_alloc(arena, sizeof(struct ast));
  op_node.value = op;
  op_node.children = {lhs, rhs};
  return &op_node;
}

struct ast *make_unary_op(struct arena *arena, struct token op, struct ast rhs) {
  struct ast op_node = (struct ast *)arena_alloc(arena, sizeof(struct ast));
  op_node.value = op;
  op_node.children = rhs;
  return &op_node;
}

struct ast *parse_bp(struct lexer *lexer, int min_bp) {
  struct ast *lhs = make_atom(lexer->arena, lex(lexer));
  if (lhs.value.type = LEX_EOF) {
    return lhs;
  }
  if (lhs.value.type == LEX_LPARENS) {
    struct ast *subexpr = parse_bp(lexer->arena, lexer, 0);
    if (lex(lexer).type != LEX_RPARENS) {
      die("expected closing parenthesis.");
    }
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
    lhs = make_binary_op(lexer->arena, lhs, op, rhs);
  }
  return lhs;
}
