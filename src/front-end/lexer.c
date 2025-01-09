#include "front-end/lexer.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

struct lexer make_lexer(char *src) {
     struct lexer lexer;
     lexer.start = src;
     lexer.current = src;
     lexer.line = 0;
     lexer.arena = make_arena();
     return lexer;
}

bool match(struct lexer *lexer, char c) {
     if (*(lexer->current + 1) == c) {
          lexer->current++;
          return 1;
     }
     return 0;
}

struct ast *make_token(struct lexer *lexer, enum token_type type) {
     struct ast token = (struct ast *)arena_alloc(lexer->arena, sizeof(struct ast));
     token.start = lexer->start;
     token.len = (int)(lexer->current - lexer->start);
     token.type = type;
     token.line = lexer->line;
     lexer->current++;
     return &token;
}

struct ast *identifier(struct lexer *lexer) {
     while (isalnum(lexer->current[1]) || lexer->current[1] == '_') {
          lexer->current++;
     }
     return make_token(lexer, LEX_IDENTIFIER);
}

struct ast *complete_keyword(struct lexer *lexer, const char *completion, enum token_type type) {
     int i = 0;
     lexer->current++;
     while (i < strlen(completion)) {
          if (*lexer->current != completion[i]) {
               return identifier(lexer);
          }
          i++;
          lexer->current++;
     }
     return make_token(lexer, type);
}

struct ast *keyword(struct lexer *lexer) {
     // void
     // bool
     // char
     // uchar
     // short
     // ushort
     // int
     // uint
     // float
     // ufloat
     // long
     // ulong
     // double
     // udouble
     // const
     // spawn
     // die
     // return
     // typedef
     // struct
     // enum
     // union
     // if
     // else
     // switch
     // case
     // for
     // while
     switch (*lexer->current) {
     case 'v': return complete_keyword(lexer, "oid", LEX_VOID);
     case 'b': return complete_keyword(lexer, "ool", LEX_BOOL);
     case 'c':
          switch(lexer->current[1]) {
          case 'h': return complete_keyword(lexer, "har", LEX_CHAR);
          case 'o': return complete_keyword(lexer, "onst", LEX_CONST);
          case 'a': return complete_keyword(lexer, "ase", LEX_CASE);
          }
          break;
     case 'u':
          switch(lexer->current[1]) {
          case 'c': return complete_keyword(lexer, "char", LEX_UCHAR);
          case 's': return complete_keyword(lexer, "short", LEX_USHORT);
          case 'i': return complete_keyword(lexer, "int", LEX_UINT);
          case 'l': return complete_keyword(lexer, "long", LEX_ULONG);
          case 'n': return complete_keyword(lexer, "nion", LEX_UNION);
          }
          break;
     case 's':
          switch (lexer->current[1]) {
          case 'h': return complete_keyword(lexer, "hort", LEX_SHORT);
          case 'w': return complete_keyword(lexer, "witch", LEX_SWITCH);
          case 't': return complete_keyword(lexer, "truct", LEX_STRUCT);
          }
          break;
     case 'i':
          switch (lexer->current[1]) {
          case 'n': return complete_keyword(lexer, "nt", LEX_INT);
          case 'f': return make_token(lexer, LEX_IF);
          }
          break;
     case 'f':
          switch (lexer->current[1]) {
          case 'l': return complete_keyword(lexer, "loat", LEX_FLOAT);
          case 'o': return complete_keyword(lexer, "or", LEX_FOR);
          }
          break;
     case 'l': return complete_keyword(lexer, "ong", LEX_LONG);
     case 'd':
          switch (lexer->current[1]) {
          case 'o': return complete_keyword(lexer, "ouble", LEX_DOUBLE);
          case 'i': return complete_keyword(lexer, "ie", LEX_DIE);
          }
          break;
     case 'r': return complete_keyword(lexer, "eturn", LEX_RETURN);
     case 't': return complete_keyword(lexer, "ypedef", LEX_TYPEDEF);
     case 'e':
          switch (lexer->current[1]) {
          case 'n': return complete_keyword(lexer, "num", LEX_ENUM);
          case 'l': return complete_keyword(lexer, "lse", LEX_ELSE);
          }
          break;
     case 'w': return complete_keyword(lexer, "hile", LEX_WHILE);
     }
     return identifier(lexer);
}

struct ast *hex(struct lexer *lexer) {
     lexer->current++;
     if (!isxdigit(*lexer->current)) {
          fprintf(stderr, "Lex error: hex literal start sequence (\"0x\") but no hex digits.\n");
          exit(1);
     }
     lexer->current++;
     while (isxdigit(*lexer->current)) {
          lexer->current++;
     }
     return make_token(lexer, LEX_HEX_LITERAL);
}

struct ast *binary(struct lexer *lexer) {
     lexer->current++;
     if (!(*lexer->current == '0' || *lexer->current == '1')) {
          fprintf(stderr, "Lex error: binary literal start sequence (\"0b\") but no binary digits.\n");
          exit(1);
     }
     lexer->current++;
     while (*lexer->current == '0' || *lexer->current == '1') {
          lexer->current++;
     }
     return make_token(lexer, LEX_BINARY_LITERAL);
}

struct ast *octal(struct lexer *lexer) {
     lexer->current++;
     if (!(*lexer->current >= '0' && *lexer->current <= '7')) {
          fprintf(stderr, "Lex error: octal literal start sequence (\"0o\") but no octal digits.\n");
          exit(1);
     }
     lexer->current++;
     while (*lexer->current >= '0' && *lexer->current <= '7') {
          lexer->current++;
     }
     return make_token(lexer, LEX_OCTAL_LITERAL);
}

struct ast *number(struct lexer *lexer) {
     if (*lexer->current == '0') {
          lexer->current++;
          switch (*lexer->current) {
          case 'x': return hex(lexer);
          case 'b': return binary(lexer);
          case 'o': return octal(lexer);
          }
     }
     while (*lexer->current >= '0' && *lexer->current <= '9') {
          lexer->current++;
     }
     if (*lexer->current == '.') {
          while (*lexer->current >= '0' && *lexer->current <= '9') {
               lexer->current++;
          }
     }
     return make_token(lexer, LEX_NUM_LITERAL);
}

struct ast *string(struct lexer *lexer) {
     lexer->current++;
     while(*lexer->current != '"') {
          if (*lexer->current == '\0') {
               fprintf(stderr, "Lex error: unterminated string literal.\n");
               exit(1);
          }
          lexer->current++;
     }
     return make_token(lexer, LEX_STR_LITERAL);
}

struct ast *char_literal(struct lexer *lexer) {
     lexer->current++;
     lexer->current += *lexer->current == '/';
     lexer->current++;
     if (*lexer->current != '\'') {
          fprintf(stderr, "Lex error: unterminated char literal.\n");
          exit(1);
     }
     return make_token(lexer, LEX_CHAR_LITERAL);
}

void skip_whitespace(struct lexer *lexer) {
     for (;;) {
          // I could make the newline case fallthrough after lexer->line++ to the tab case, but it would make the code confusing.
          switch (*lexer->current) {
          case ' ':
          case '\t':
               lexer->current++;
               break;
          case '\n':
               lexer->line++;
               lexer->current++;
               break;
          default:
               return;
          }
     }
}

struct ast *lex(struct lexer *lexer) {
     skip_whitespace(lexer);
     if (*lexer->current == '\0') {
          return make_token(lexer, LEX_EOF);
     }
     lexer->start = lexer->current;
     char c = *lexer->start;
     if (isalpha(c)) return keyword(lexer);
     if (isdigit(c)) return number(lexer);
     if (c == '"') return string(lexer);
     if (c == '\'') return char_literal(lexer);

     switch (c) {
     case ';': return make_token(lexer, LEX_SEMICOLON);
     case '(': return make_token(lexer, LEX_LPARENS);
     case ')': return make_token(lexer, LEX_RPARENS);
     case '[': return make_token(lexer, LEX_LBRACKETS);
     case ']': return make_token(lexer, LEX_RBRACKETS);
     case '{': return make_token(lexer, LEX_LCURLY_BRACE);
     case '}': return make_token(lexer, LEX_RCURLY_BRACE);

     case '=': return make_token(lexer, match(lexer, '=') ? LEX_EQUALS : LEX_ASSIGNMENT);
     case '+':
          if (match(lexer, '+')) return make_token(lexer, LEX_INCREMENT);
          if (match(lexer, '=')) return make_token(lexer, LEX_PLUS_EQUALS);
          return make_token(lexer, LEX_PLUS);
     case '-':
          if (match(lexer, '-')) return make_token(lexer, LEX_DECREMENT);
          if (match(lexer, '=')) return make_token(lexer, LEX_MINUS_EQUALS);
          return make_token(lexer, LEX_MINUS);
     case '*': return make_token(lexer, match(lexer, '=') ? LEX_STAR_EQUALS : LEX_STAR);
     case '/': return make_token(lexer, match(lexer, '=') ? LEX_SLASH_EQUALS : LEX_SLASH);
     case '%': return make_token(lexer, match(lexer, '=') ? LEX_MOD_EQUALS : LEX_MOD);
     case '&':
          if (match(lexer, '&')) return make_token(lexer, LEX_AND);
          if (match(lexer, '=')) return make_token(lexer, LEX_BITWISE_AND_EQUALS);
          return make_token(lexer, LEX_BITWISE_AND);
     case '|':
          if (match(lexer, '|')) return make_token(lexer, LEX_OR);
          if (match(lexer, '=')) return make_token(lexer, LEX_BITWISE_OR_EQUALS);
          return make_token(lexer, LEX_BITWISE_OR);
     case '^': return make_token(lexer, match(lexer, '=') ? LEX_BITWISE_XOR_EQUALS : LEX_BITWISE_XOR);
     case '~': return make_token(lexer, match(lexer, '=') ? LEX_BITWISE_NOT_EQUALS : LEX_BITWISE_NOT);
     case '<':
          if (match(lexer, '=')) return make_token(lexer, LEX_LESS_THAN_OR_EQUAL_TO);
          if (match(lexer, '<')) {
               if (match(lexer, '=')) return make_token(lexer, LEX_LSHIFT_EQUALS);
               return make_token(lexer, LEX_LSHIFT);
          }
          return make_token(lexer, LEX_LESS_THAN);
     case '>':
          if (match(lexer, '=')) return make_token(lexer, LEX_GREATER_THAN_OR_EQUAL_TO);
          if (match(lexer, '>')) {
               if (match(lexer, '=')) return make_token(lexer, LEX_RSHIFT_EQUALS);
               return make_token(lexer, LEX_RSHIFT);
          }
          return make_token(lexer, LEX_GREATER_THAN);
     }
     fprintf(stderr, "Lex error: unknown character '%c'.\n", *lexer->current);
     exit(1);
}

// *char s = [char]alloc(10)
