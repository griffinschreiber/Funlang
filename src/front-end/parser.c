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
  case LEX_CHAR_LITERAL:
  case LEX_STR_LITERAL:
    return 0;
  case LEX_COMMA:
    return 1;
  case LEX_OR:
  case LEX_AND:
  case LEX_EQUALS:
  case LEX_NOT_EQUALS:
    return 2;
  case LEX_LESS_THAN:
  case LEX_GREATER_THAN:
  case LEX_LESS_THAN_OR_EQUAL_TO:
  case LEX_GREATER_THAN_OR_EQUAL_TO:
    return 3;
  case LEX_LSHIFT:
  case LEX_RSHIFT:
    return 4;
  case LEX_PLUS:
  case LEX_MINUS:
    return 5;
  case LEX_STAR:
  case LEX_SLASH:
  case LEX_MOD:
    return 6;
  case LEX_BITWISE_AND:
  case LEX_BITWISE_OR:
  case LEX_BITWISE_XOR:
    return 7;
  }
}

int get_rbp(enum token_type type) {
  switch (type) {
    case LEX_EOF:
    case LEX_SEMICOLON:
    case LEX_IDENTIFIER:
    case LEX_NUM_LITERAL:
    case LEX_CHAR_LITERAL:
    case LEX_STR_LITERAL:
      return 0;
    case LEX_COMMA:
      return 2;
    case LEX_OR:
    case LEX_AND:
    case LEX_EQUALS:
    case LEX_NOT_EQUALS:
      return 3;
    case LEX_LSHIFT:
    case LEX_RSHIFT:
      return 4;
    case LEX_PLUS:
    case LEX_MINUS:
      return 5;
    case LEX_STAR:
    case LEX_SLASH:
    case LEX_MOD:
      return 6;
    case LEX_BITWISE_AND:
    case LEX_BITWISE_OR:
    case LEX_BITWISE_XOR:
      return 7;
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

struct ast *parse_bp(struct arena *arena, struct lexer *lexer, int min_bp) {
  if (advance) {
    struct ast *lhs = lexer->next;
  }
  if (lhs.value.type = LEX_EOF) {
    return lhs;
  }
  if (lhs.value.type == LEX_LPARENS) {
    struct ast *subexpr = parse_bp(arena, lexer, 0);
    if (lex(lexer).type != LEX_RPARENS) {
      die("expected closing parenthesis.");
    }
  }
  int lhs_rbp = get_rbp(lhs.value.type);
  if (lhs_rbp) {
    struct ast rhs = parse_bp(arena, lexer, lhs_rbp);
    return make_unary_op(arena, lhs.value, rhs);
  }
  for (;;) {
    struct token op = lex(lexer);
    if (op.type = LEX_EOF) {
      break;
    }
    advance(lexer);
    int l_bp = get_lbp(token.type);
    int r_bp = get_rbp(token.type);
    if (l_bp < min_bp) {
      break;
    }
    struct ast rhs = parse_bp(arena, lexer, r_bp);
    lhs = make_binary_op(arena, lhs, op, rhs);
  }
  return lhs;
}
