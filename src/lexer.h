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
  kTokenAssign,
  kTokenEquals,
  kTokenNot,
  kTokenNotEquals,
  kTokenLessThan,
  kTokenLessOrEquals,
  kTokenGreaterThan,
  kTokenGreaterOrEquals,
  kTokenLeftParenthesis,
  kTokenRightParenthesis,
  kTokenString,
  kTokenIf,
  kTokenElse,
  kTokenEndif,
  kTokenFor,
  kTokenEndfor,
  kTokenPrint,
  kTokenBoolean
} TokenType;

typedef enum {
  kPrecNone,
  kPrecAssignment,  // =
  kPrecOr,          // ||
  kPrecAnd,         // &&
  kPrecEquality,    // ==, !=
  kPrecComparison,  // <, >, <=, >=
  kPrecTerm,        // +, -
  kPrecFactor,      // *, /
  kPrecUnary,       // !, - (prefix)
  kPrecCall,        // Function calls
  kPrecPrimary      // Literals, variables, (expr)
} Precedence;

typedef struct Token {
  TokenType type;
  union {
    int number;
    char text[kTokenTextBufferSize];
  } value;
  const char* start_of_token;
  Precedence precedence;
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
