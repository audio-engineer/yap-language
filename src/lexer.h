#ifndef LEXER_H
#define LEXER_H

#ifdef __CC65__
enum Constants {
  kTokenTextBufferSize = 100,
};
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
  kTokenEquals,
  kTokenLeftParenthesis,
  kTokenRightParenthesis,
  kTokenQuotationMark,
  kTokenIf,
  kTokenFor,
  kTokenEndfor,
  kTokenPrint
} TokenType;

typedef struct Token {
  TokenType type;
  long value;
  char text[kTokenTextBufferSize];
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
