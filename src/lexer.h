#ifndef LEXER_H
#define LEXER_H

typedef enum TokenType {
  kTokenEof,
  kTokenId,
  kTokenNumber,
  kTokenPlus,
  kTokenMinus,
  kTokenStar,
  kTokenSlash,
  kTokenEquals,
  kTokenLessThan,
  kTokenGreaterThan,
  kTokenLeftParenthesis,
  kTokenRightParenthesis,
  kTokenQuotationMark,
  kTokenIf,
  kTokenFor,
  kTokenEndfor,
  kTokenPrint,
  kTokenTrue,
  kTokenFalse
} TokenType;

static constexpr int kTokenTextBufferSize = 100;

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
