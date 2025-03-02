#ifndef LEXER_H
#define LEXER_H

enum {
  kTokenTextBufferSize = 100,
};

typedef enum TokenType {
  kEOF,
  kId,
  kNumber,
  kPlus,
  kMinus,
  kStar,
  kSlash,
  kAssignment,
  kSemicolon,
  kLeftParenthesis,
  kRightParenthesis,
  kQuotationMark,
  kIf,
  kPrint
} TokenType;

typedef struct Token {
  TokenType type;
  int value;
  char text[kTokenTextBufferSize];
} Token;

// NOLINTBEGIN(cppcoreguidelines-avoid-non-const-global-variables)
extern const char* source_code;
extern Token token;
// NOLINTEND(cppcoreguidelines-avoid-non-const-global-variables)

void ConsumeNextToken();

#endif  // LEXER_H
