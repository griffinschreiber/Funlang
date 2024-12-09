#include "front-end/lexer.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// must be greater than 2.
#define LEX_SCRATCHPAD_SIZE 128

// Macros for lexer utils.
#define current (*(lexer->start + lexer->len))
#define peek (*(lexer->start + lexer->len + 1))
#define peekn(n) (*(lexer->start + lexer->len + n))
#define match(c) (peek == c)
#define isbdigit(c) (c == '0' || c == '1')
#define isodigit(c) (c >= '0' && c <= '7')

void die(const char msg[]) {
     fprintf(stderr, "Lex error: %s\n", msg);
     exit(1);
}

struct token {
     char *value;
     enum token_type type;
     int line;
};

struct lexer {
     char *src;
     char *start;
     int len;
     int line;
     char scratchpad[LEX_SCRATCHPAD_SIZE];
     struct token token_buffer;
};

struct token *make_token(struct lexer *lexer, enum token_type type) {
     struct token *token = &lexer->token_buffer;
     // length of string lexer->start to lexer->start + lexer->len is lexer->len + 1. don't count null terminator
     // in the ternary condition bc it's a "<" and not a "<=".
     token->value = (lexer->len + 1 < LEX_SCRATCHPAD_SIZE) ? &lexer->scratchpad : (char *)malloc(lexer->len + 2);
     token->value[lexer->len] = '\0';
     strncat(token->value, lexer->start, lexer->len);
     token->type = type;
     token->line = lexer->line;
     lexer->start = lexer->start + lexer->len;
     return token;
}

struct token *make_char_token(struct lexer *lexer, char c) {
     struct token *token = &lexer->token_buffer;
     token->value = &lexer->scratchpad;
     token->value[0] = c;
     token->value[1] = '\0';
     token->type = LEX_CHAR_LITERAL;
     token->line = lexer->line;
     lexer->start = lexer->start + lexer->len;
     return token;
}

struct token *make_str_token(struct lexer *lexer, char *buf) {
     struct token *token = &lexer->token_buffer;
     int buf_len = strlen(buf);
     token->value = (buf_len < LEX_SCRATCHPAD_SIZE) ? &lexer->scratchpad : (char *)malloc(buf_len + 1);
     token->value[buf_len + 1] = '\0';
     strncat(token->value, buf, strlen(token->value));
     token->type = LEX_STR_LITERAL;
     token->line = lexer->line;
     lexer->start = lexer->start + lexer->len;
     free(buf);
     return token;
}

void skip_comment(struct lexer *lexer) {
     while (peek != '\0' && peek != '\n') {
          lexer->start++;
     }
     lexer->line++;
}

void skip_block_comment(struct lexer *lexer) {
     while (peek != '\0') {
          lexer->start++;
          if (current == '\n') {
               lexer->line++;
          } else if (current == '*' && match('/')) {
               break;
          }
     }
}

void skip_whitespace(struct lexer *lexer) {
     for (;;) {
          switch (peek) {
          case '/':
               if (match('/')) {
                    skip_comment(lexer);
               } else if (match('*')) {
                    skip_block_comment(lexer);
               } else {
                    return;
               }
          case ' ':
          case '\t':
          case '\n':
               lexer->line++;
               lexer->start++;
               break;
          default:
               return;
          }
     }
}

struct token *hex(struct lexer *lexer) {
     lexer->len++;
     while (isxdigit(peek)) {
          lexer->len++;
     }
     return make_token(lexer, LEX_NUM_LITERAL);
}

struct token *binary(struct lexer *lexer) {
     lexer->len++;
     while (isbdigit(peek)) {
          lexer->len++;
     }
     return make_token(lexer, LEX_NUM_LITERAL);
}

struct token *octal(struct lexer *lexer) {
     lexer->len++;
     while (isodigit(peek)) {
          lexer->len++;
     }
     return make_token(lexer, LEX_NUM_LITERAL);
}

// 42
// 3.14
// 0x2f9d5c
// 0b011011
// 0o777
struct token *number(struct lexer *lexer) {
     char c = *(lexer->start + lexer->len);
     if (c == '0') {
          switch (c) {
          case 'x': return hex(lexer);
          case 'b': return binary(lexer);
          case 'o': return octal(lexer);
          }
     }
     lexer->len++;
     while (isdigit(current)) { lexer->len++; }
     if (current == '.') {
          while (isdigit(current)) { lexer->len++; }
     }
     return make_token(lexer, LEX_NUM_LITERAL);
}

// NOTE: the compiler will only interpret the first two hex digits (one character in utf-8) after the escape character.
char interpret_hex(struct lexer *lexer) {
     const char *hex_start = lexer->start + lexer->len;
     lexer->len++;
     int len = 0;
     while (isxdigit(peek)) {
          lexer->len++;
          len++;
     }
     if (!len) {
          die("hex escape character (\\x), but no hex digits.");
     }
     // It's 3 because it needs space for a null terminator.
     char buf[3];
     buf[2] = '\0';
     strncat(buf, hex_start, 2);
     // hope that we got it right and it is definitely hex, otherwise undefined behaviour.
     return (char)strtol(buf, NULL, 16);
}

// NOTE: the compiler will only interpret the first three octal digits (one character in utf-8) after the escape character. if the escape sequence has a value greater than 0xff (>0377) it will die.
char interpret_octal(struct lexer *lexer) {
     const char *octal_start = lexer->start + lexer->len;
     lexer->len++;
     int len = 0;
     while (isodigit(peek)) {
          lexer->len++;
          len++;
     }
     if (!len) {
          die("octal escape character (\\o), but no octal digits.");
     }
     // It's 4 because it needs space for a null terminator.
     char buf[4];
     buf[3] = '\0';
     strncat(buf, octal_start, 3);
     // hope that we got it right and it is definitely octal, otherwise undefined behaviour.
     long l = strtol(buf, NULL, 8);
     if (l > 255) {
          die("octal escape sequence with value greater than 0xff (>0377).");
     }
     return (char)l;
}

char escape_char(struct lexer *lexer) {
     lexer->len++;
     switch (current) {
     case '\'':
     case '"':
     case '\\':
          return current;
     case 'a': return '\a';
     case 'b': return '\b';
     case 'f': return '\f';
     case 'n': return '\n';
     case 'r': return '\r';
     case 't': return '\t';
          // what does e do? should it be part of the standard?
     case 'e': return '\033';
     case 'x': return interpret_hex(lexer);
     case 'o': return interpret_octal(lexer);
     default:
          die("unrecognized escape character.");
     }
     // this should never happen.
     // to do: unreachable() function.
     exit(1);
}

// to do: use some sort of buffer with pointer resetting so I can malloc less.
struct token *rstring(struct lexer *lexer) {
     ulong seek = 0;
     // to do: do this nicely.
     while (*(lexer->start + lexer->len + seek) != '\0') {
          if (*(lexer->start + lexer->len + seek) == '\"') {
               break;
          }
          seek++;
     }
     char *buf = (seek + 1 < LEX_SCRATCHPAD_SIZE) ? &lexer->scratchpad : (char *)malloc(seek + 2);
     int i = 0;
     for (;;) {
          lexer->len++;
          if (current == '\0') {
               die("unterminated rstring.");
          }
          if (current == '\"') {
               break;
          }
          if (current == '\\') {
               continue;
          }
          buf[i] = current;
          i++;
     }
     buf[seek + 1] = '\0';
     return make_str_token(lexer, buf);
}

struct token *string(struct lexer *lexer) {
     ulong seek = 0;
     // to do: do this nicely.
     while (*(lexer->start + lexer->len + seek) != '\0') {
          if (*(lexer->start + lexer->len + seek) == '\"') {
               break;
          }
          seek++;
     }
     char *buf = (seek + 1 < LEX_SCRATCHPAD_SIZE) ? &lexer->scratchpad : (char *)malloc(seek + 2);
     int i = 0;
     for (;;) {
          lexer->len++;
          if (current == '\0') {
               die("unterminated string.");
          }
          if (current == '\"') {
               break;
          }
          if (current == '\\') {
               buf[i] = escape_char(lexer);
               i++;
          }
     }
     return make_str_token(lexer, buf);
}

struct token *char_literal(struct lexer *lexer) {
     lexer->len++;
     char raw_char = (current == '\\') ? escape_char(lexer) : current;
     lexer->len++;
     if (current != '\'') {
          die("char literal longer than one char.");
     }
     return make_char_token(lexer, raw_char);
}

struct token *identifier(struct lexer *lexer) {
     while (isalnum(peek) || peek == '_') {
          lexer->len++;
     }
     return make_token(lexer, LEX_IDENTIFIER);
}

struct token *complete_keyword(struct lexer *lexer, const char completion[], enum token_type token_type) {
     ulong i = 0;
     while (i < strlen(completion)) {
          i++;
          lexer->len++;
          if (current != *(completion + i)) {
               return identifier(lexer);
          }
     }

     return make_token(lexer, token_type);
}

struct token *keyword(struct lexer *lexer) {
     // void
     // bool
     // char
     // short
     // ushort
     // int
     // uint
     // float
     // long
     // ulong
     // double
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
     switch (current) {
     case 'v': return complete_keyword(lexer, "oid", LEX_VOID);
     case 'b': return complete_keyword(lexer, "ool", LEX_BOOL);
     case 'c':
          switch (peek) {
          case 'h': return complete_keyword(lexer, "har", LEX_CHAR);
          case 'o': return complete_keyword(lexer, "onst", LEX_CONST);
          case 'a': return complete_keyword(lexer, "ase", LEX_CASE);
          default: return identifier(lexer);
          }
     case 's':
          switch (peek) {
          case 'h': return complete_keyword(lexer, "hort", LEX_SHORT);
          case 'p': return complete_keyword(lexer, "pawn", LEX_SPAWN);
          case 'w': return complete_keyword(lexer, "witch", LEX_SWITCH);
          default: return identifier(lexer);
          }
     case 'u':
          switch (peek) {
          case 's': return complete_keyword(lexer, "short", LEX_USHORT);
          case 'i': return complete_keyword(lexer, "int", LEX_UINT);
          case 'l': return complete_keyword(lexer, "long", LEX_ULONG);
          case 'n':
               switch (peekn(2)) {
               case 'i': return complete_keyword(lexer, "nion", LEX_UNION);
               case 'l': return complete_keyword(lexer, "nless", LEX_UNLESS);
               default: return identifier(lexer);
               }
          default: return identifier(lexer);
          }
     case 'i':
          switch (peek) {
          case 'n': return complete_keyword(lexer, "nt", LEX_INT);
          case 'f': return make_token(lexer, LEX_IF);
          default: return identifier(lexer);
          }
     case 'f':
          switch (peek) {
          case 'l': return complete_keyword(lexer, "loat", LEX_FLOAT);
          case 'o': return complete_keyword(lexer, "or", LEX_FOR);
          default: return identifier(lexer);
          }
     case 'l': return complete_keyword(lexer, "ong", LEX_LONG);
     case 'd':
          switch (peek) {
          case 'o': return complete_keyword(lexer, "ouble", LEX_DOUBLE);
          case 'i': return complete_keyword(lexer, "ie", LEX_DIE);
          default: return identifier(lexer);
          }
     case 'r':
          switch (peek) {
          case 'e': return complete_keyword(lexer, "eturn", LEX_RETURN);
          case '\"': return rstring(lexer);
          }
     case 't': return complete_keyword(lexer, "ypedef", LEX_TYPEDEF);
     case 'e':
          switch (peek) {
          case 'n': return complete_keyword(lexer, "num", LEX_ENUM);
          case 'l': return complete_keyword(lexer, "lse", LEX_ELSE);
          default: return identifier(lexer);
          }
     case 'w': return complete_keyword(lexer, "hile", LEX_WHILE);
     default: return identifier(lexer);
     }
}

struct token *lex(struct lexer *lexer) {
     if (lexer->len > strlen(lexer->start)) {
          die("lex called after emitting end of file.");
     }
     skip_whitespace(lexer);
     if (current == '\0') {
          return make_token(lexer, LEX_EOF);
     }

     if (isalpha(current)) return keyword(lexer);
     if (isdigit(current)) return number(lexer);
     if (current == '\"') return string(lexer);
     if (current == '\'') return char_literal(lexer);

     switch (current) {
     case ';': return make_token(lexer, LEX_SEMICOLON);
     case '(': return make_token(lexer, LEX_LPARENS);
     case ')': return make_token(lexer, LEX_RPARENS);
     case '[': return make_token(lexer, LEX_LBRACKETS);
     case ']': return make_token(lexer, LEX_RBRACKETS);
     case '{': return make_token(lexer, LEX_LCURLY_BRACE);
     case '}': return make_token(lexer, LEX_RCURLY_BRACE);

     case '+':
          if (match('+')) return make_token(lexer, LEX_INCREMENT);
          if (match('=')) return make_token(lexer, LEX_PLUS_EQUALS);
          return make_token(lexer, LEX_PLUS);
     case '-':
          if (match('-')) return make_token(lexer, LEX_DECREMENT);
          if (match('=')) return make_token(lexer, LEX_MINUS_EQUALS);
          return make_token(lexer, LEX_MINUS);
     case '*': return match('=') ? make_token(lexer, LEX_STAR_EQUALS) : make_token(lexer, LEX_STAR);
     case '/':
          return match('=') ? make_token(lexer, LEX_SLASH_EQUALS) : make_token(lexer, LEX_SLASH);
     case '%': return match('=') ? make_token(lexer, LEX_MOD_EQUALS) : make_token(lexer, LEX_MOD);
     case '&':
          if (match('&')) return make_token(lexer, LEX_AND);
          if (match('=')) return make_token(lexer, LEX_BITWISE_AND_EQUALS);
          return make_token(lexer, LEX_BITWISE_AND);
     case '|':
          if (match('|')) return make_token(lexer, LEX_OR);
          if (match('=')) return make_token(lexer, LEX_BITWISE_OR_EQUALS);
          return make_token(lexer, LEX_BITWISE_OR);
     case '^': return match('=') ? make_token(lexer, LEX_BITWISE_XOR_EQUALS) : make_token(lexer, LEX_BITWISE_XOR);
     case '~': return match('=') ? make_token(lexer, LEX_BITWISE_NOT_EQUALS) : make_token(lexer, LEX_BITWISE_NOT);
     case '<':
          if (match('=')) return make_token(lexer, LEX_LESS_THAN_OR_EQUAL_TO);
          if (match('<')) return match('=') ? make_token(lexer, LEX_LSHIFT_EQUALS) : make_token(lexer, LEX_LSHIFT);
          return make_token(lexer, LEX_LESS_THAN);
     case '>':
          if (match('=')) return make_token(lexer, LEX_GREATER_THAN_OR_EQUAL_TO);
          if (match('<')) return match('=') ? make_token(lexer, LEX_RSHIFT_EQUALS) : make_token(lexer, LEX_RSHIFT);
          return make_token(lexer, LEX_GREATER_THAN);
     default:
          // to do: variadic stuff for die, so I can do formatting. or something.
          die("unknown character.");
     }
     // this should never happen.
     exit(1);
}

// *char s = [char]alloc(10)
