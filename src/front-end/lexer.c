#include "front-end/lexer.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

bool match(struct lexer *lexer, char c) {
     if (*(lexer->current + 1) == c) {
          lexer->current++;
          return 1;
     }
     return 0;
}

struct token make_token(struct lexer *lexer, enum token_type type) {
     struct token token;
     token.start = lexer->start;
     token.len = (int)(current - start);
     token.line = lexer->line;
     return token;
}

struct token identifier(struct lexer *lexer) {}

struct token complete_keyword(struct lexer *lexer, const char *completion, struct token_type type) {
     size_t completion_len = strlen(completion);
     if (strncmp(lexer->current, completion, completion_len) == 0) {
          lexer->current += completion_len;
          return make_token(lexer, type);
     }
     return identifier(lexer);
}

struct token keyword(struct lexer *lexer) {
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
     // unless
     // switch
     // case
     // for
     // while
     switch (*lexer->current) {
     case 'v': return complete_keyword(lexer, "oid", LEX_VOID);
     case 'b': return complete_keyword(lexer, "ool", LEX_BOOL);
     case 'c':
          switch(*(lexer->current + 1)) {
          case 'h': return complete_keyword(lexer, "har", LEX_CHAR);
          case 'o': return complete_keyword(lexer, "onst", LEX_CONST);
          case 'a': return complete_keyword(lexer, "ase", LEX_CASE);
          }
          break;
     case 'u':
          switch(*(lexer->current + 1)) {
          case 'c': return complete_keyword(lexer, "char", LEX_UCHAR);
          case 's': return complete_keyword(lexer, "short", LEX_USHORT);
          case 'i': return complete_keyword(lexer, "uint", LEX_UINT);
          case 'l': return complete_keyword(lexer, "ulong", LEX_ULONG);
          }
          break;
     case 's':
          switch (*(lexer->current + 1)) {
          case 'h': return complete_keyword(lexer, "hort", LEX_SHORT);
          case 'w': return complete_keyword(lexer, "witch", LEX_SWITCH);
          }
          break;
     case 'i':
          switch (*(lexer->current + 1)) {
          case 'n': return complete_keyword(lexer, "nt", LEX_INT);
          case 'f': return make_token(lexer, LEX_IF);
          }
          break;
     }
}

struct token hex(struct lexer *lexer) {
     lexer->current++;
     if (!isxdigit(*lexer->current)) {
          fprintf("Lex error: hex literal start sequence (\"0x\") but no hex digits.\n");
          exit(1);
     }
     lexer->current++;
     while (isxdigit(*lexer->current)) {
          lexer->current++;
     }
     return make_token(lexer, LEX_HEX_LITERAL);
}

struct token binary(struct lexer *lexer) {

}

struct token octal(struct lexer *lexer) {

}

struct token number(struct lexer *lexer) {
     if (*lexer->current == '0') {
          lexer->current++;
          switch (*lexer->current) {
          case 'x': return hex(lexer);
          case 'b': return binary(lexer);
          case 'o': return octal(lexer);
          }
          while (isnum(*lexer->current)) {
               lexer->current++;
          }
          if (*lexer->current == '.') {
               while (isnum(*lexer->current)) {
                    lexer->current++;
               }
          }
          return make_token(lexer, LEX_NUM_LITERAL);
     }

     struct token string(struct lexer *lexer) {
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

     struct token char_literal(struct lexer *lexer) {
          lexer->current++;
          lexer->current += *lexer->current == '/';
          lexer->current++;
          if (*lexer->current != '\'') {
               fprintf(stderr, "Lex error: unterminated char literal.\n");
               exit(1);
          }
          return make_token(lexer, LEX_CHAR_LITERAL);
     }

     struct token lex(struct lexer *lexer) {
          skip_whitespace(lexer);
          lexer->current = lexer->start;
          if (*lexer->current == '\0') {
               return make_token(lexer, LEX_EOF);
          }
          lexer->current++;
          char c = *lexer->current;
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

          case '=': return make_token(match(lexer, '=') ? LEX_EQUALS : LEX_ASSIGNMENT);
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
          case '^': return make_token(lexer, match(lexer, '=') ? LEX_BITWISE_XOR_EQUALS, LEX_BITWISE_XOR);
          case '~': return make_token(lexer, match(lexer, '=') ? LEX_BITWISE_NOT_EQUALS, LEX_BITWISE_NOT);
          case '<':
               if (match(lexer, '=')) return make_token(lexer, LEX_LESS_THAN_OR_EQUAL_TO);
               if (match(lexer, '<')) return make_token(lexer, LEX_LSHIFT);
               return make_token(lexer, LEX_LESS_THAN);
          case '>':
               if (match(lexer, '=')) return make_token(lexer, LEX_GREATER_THAN_OR_EQUAL_TO);
               if (match(lexer, '>')) return make_token(lexer, LEX_RSHIFT);
               return make_token(lexer, LEX_GREATER_THAN);
          }
          fprintf(stderr, "Lex error: unknown token \"%c\".\n", *lexer->current);
          exit(1);
     }

     // *char s = [char]alloc(10)
