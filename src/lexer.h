#ifndef LEXER_H
#define LEXER_H

#ifdef __CC65__
#include <stdbool.h>
#endif

#ifdef __CC65__
enum { kTokenTextBufferSize = 100 };
#else
static constexpr int kTokenTextBufferSize = 100;
#endif

typedef enum TokenType {
  kTokenEof,
  kTokenId,
  kTokenNumber,
  kTokenPlus,
  kTokenMinus,
  kTokenStar,
  kTokenSlash,
  kTokenColon,
  kTokenEquals,
  kTokenLessThan,
  kTokenLessOrEquals,
  kTokenGreaterThan,
  kTokenGreaterOrEquals,
  kTokenLeftParenthesis,
  kTokenRightParenthesis,
  kTokenString,
  kTokenIf,
  kTokenEndif,
  kTokenFor,
  kTokenEndfor,
  kTokenPrint,
  kTokenBoolean
} TokenType;

typedef struct Token {
  TokenType type;
  union {
    long number;
    char text[kTokenTextBufferSize];
  } value;
} Token;

// NOLINTBEGIN(cppcoreguidelines-avoid-non-const-global-variables)
extern const char* source_code;
extern Token token;
// NOLINTEND(cppcoreguidelines-avoid-non-const-global-variables)

/**
 * Parse tokens.
 */
void ConsumeNextToken();

#endif  // LEXER_H
