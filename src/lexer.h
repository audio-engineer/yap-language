#ifndef LEXER_H
#define LEXER_H

typedef enum TokenType {
  NUMBER,
  PLUS,
  MINUS,
  MULTIPLICATION,
  DIVISION,
  ASSIGNMENT,
  SEMICOLON,
  PRINT
} TokenType;

typedef struct Token {
  TokenType tokenType;
  int value;
  char text[100];
} Token;

#endif  // LEXER_H
