#ifndef LEXER_H
#define LEXER_H

enum {
  kTokenTextBufferSize = 100,
};

typedef enum TokenType {
  kNumber,
  kPlus,
  kMinus,
  kMultiplication,
  kDivision,
  kAssignment,
  kSemicolon,
  kPrint
} TokenType;

typedef struct Token {
  TokenType token_type;
  int value;
  char text[kTokenTextBufferSize];
} Token;

#endif  // LEXER_H
