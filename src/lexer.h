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

/// Checks if the current token is one of the argument tokens, and, if true,
/// consumes the token and returns true.
#define AcceptToken(token_type_list_length, ...) \
  (AcceptTokenImplementation(token_type_list_length, __VA_ARGS__))

/// Checks if the current token is one of the argument tokens, and, if true,
/// consumes the token and returns true.
/// If false, an error is emitted.
#define ExpectToken(token_type_list_length, ...)    \
  (AcceptToken(token_type_list_length, __VA_ARGS__) \
       ? true                                       \
       : (puts("Error: Unexpected token."), false))

#ifdef __clang__
// NOLINTNEXTLINE(bugprone-reserved-identifier)
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
  kTokenPercent,
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
  kTokenBool,
  kTokenLeftBracket,
  kTokenRightBracket,
  kTokenArray
} TokenType;

typedef struct Token {
  TokenType type;
  union value {
    int number;
    char text[kTokenTextBufferSize];
  } value;
} Token;

// NOLINTBEGIN(cppcoreguidelines-avoid-non-const-global-variables)
extern Token token;
extern char program_buffer[];
extern size_t program_buffer_index;
// NOLINTEND(cppcoreguidelines-avoid-non-const-global-variables)

bool __cdecl__ AcceptTokenImplementation(size_t token_type_list_length, ...);

void ResetLexerState();

void ExtractIdentifierName(char* buffer);

/// Parse tokens.
void ConsumeNextToken();

#endif  // LEXER_H
