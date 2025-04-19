#ifndef LEXER_H
#define LEXER_H

#ifdef __CC65__
#include <stdbool.h>
#endif

#if defined(__CC65__) || defined(__linux__)
#include <stddef.h>
#elif __APPLE__
#include <sys/_types/_size_t.h>
#endif

#if !defined(__CC65__)
#define __cdecl__
#endif

#ifdef __CC65__
enum { kTokenTextBufferSize = 100, kProgramBufferSize = 8192 };
#else
static constexpr int kTokenTextBufferSize = 100;
static constexpr int kProgramBufferSize = 8192;
#endif

typedef enum TokenType {
  kTokenEof,
  kTokenIdentifier,
  kTokenNumber,
  kTokenPlus,
  kTokenMinus,
  kTokenStar,
  kTokenSlash,
  kTokenDot,
  kTokenComma,
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
  kTokenLocal,
  kTokenBoolean,
  kTokenFunc,
  kTokenEndfunc,
  kTokenRet,
  kTokenInt,
  kTokenFloat,
  kTokenStr,
  kTokenBool
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
  size_t start_of_token;
  Precedence precedence;
} Token;

// NOLINTBEGIN(cppcoreguidelines-avoid-non-const-global-variables)
extern Token token;
extern Token next_token;
extern char program_buffer[];
extern size_t program_buffer_index;
// NOLINTEND(cppcoreguidelines-avoid-non-const-global-variables)

bool __cdecl__ AcceptToken(size_t token_type_list_length, ...);

bool __cdecl__ ExpectToken(size_t token_type_list_length, ...);

void ResetLexerState();

void ExtractIdentifierName(char* buffer);

bool IsBinaryOperator(TokenType token_type);

/// Parse tokens.
void ConsumeNextToken();

void PeekNextToken();

#endif  // LEXER_H
