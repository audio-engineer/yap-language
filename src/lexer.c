#include "lexer.h"

#include <ctype.h>
#ifdef __CC65__
#include <stdbool.h>
#endif
#if defined(__CC65__) || defined(__linux__)
#include <stddef.h>
#elif __APPLE__
#include <sys/_types/_size_t.h>
#endif
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// NOLINTBEGIN(cppcoreguidelines-avoid-non-const-global-variables)
Token token;
Token next_token;
char program_buffer[kProgramBufferSize];
size_t program_buffer_index = 0;
// NOLINTEND(cppcoreguidelines-avoid-non-const-global-variables)

typedef struct KeywordEntry {
  const char* const kText;
  const TokenType kType;
} KeywordEntry;

static const KeywordEntry kKeywordMap[] = {
    {"print", kTokenPrint}, {"if", kTokenIf},     {"else", kTokenElse},
    {"endif", kTokenEndif}, {"for", kTokenFor},   {"endfor", kTokenEndfor},
    {"local", kTokenLocal}, {"func", kTokenFunc}, {"endfunc", kTokenEndfunc},
    {"ret", kTokenRet},     {"int", kTokenInt},   {"float", kTokenFloat},
    {"str", kTokenStr},     {"bool", kTokenBool}};

#ifdef __CC65__
static const size_t kKeywordCount = sizeof(kKeywordMap) / sizeof(KeywordEntry);
#else
static constexpr size_t kKeywordCount =
    sizeof(kKeywordMap) / sizeof(KeywordEntry);
#endif

void ResetLexerState() {
  memset(program_buffer, 0, kProgramBufferSize);

  program_buffer_index = 0;
}

static void IncrementProgramBufferIndex() {
  if (kProgramBufferSize < program_buffer_index) {
    puts("Error: program buffer overflow! Too many program lines!");

    return;
  }

  ++program_buffer_index;
}

static void DecrementProgramBufferIndex() {
  if (0 == program_buffer_index) {
    puts("Error: Unable to decrement, error in lexer");

    return;
  }

  --program_buffer_index;
}

bool __cdecl__ AcceptTokenWrapper(const size_t token_type_list_length,
                                  va_list args) {
  size_t index = 0;

  for (index = 0; index < token_type_list_length; ++index) {
    const TokenType kTokenType = va_arg(args, TokenType);

    if (kTokenType == token.type) {
      ConsumeNextToken();

      va_end(args);

      return true;
    }
  }

  va_end(args);

  return false;
}

bool __cdecl__ AcceptToken(const size_t token_type_list_length, ...) {
  va_list args = NULL;
  size_t index = 0;

  va_start(args, token_type_list_length);

  for (index = 0; index < token_type_list_length; ++index) {
    const TokenType kTokenType = va_arg(args, TokenType);

    // printf("tt:%d, t:%d\n", kTokenType, token.type);

    if (kTokenType == token.type) {
      ConsumeNextToken();

      va_end(args);

      return true;
    }
  }

  va_end(args);

  return false;
}

bool __cdecl__ ExpectToken(const size_t token_type_list_length, ...) {
  va_list args = NULL;
  size_t index = 0;

  va_start(args, token_type_list_length);

  for (index = 0; index < token_type_list_length; ++index) {
    const TokenType kTokenType = va_arg(args, TokenType);

    // printf("tt:%d, t:%d\n", kTokenType, token.type);

    if (kTokenType == token.type) {
      ConsumeNextToken();

      va_end(args);

      return true;
    }
  }

  va_end(args);

  puts("Error: Unexpected token.");

  return false;
}

static void SkipWhitespace() {
  while (program_buffer[program_buffer_index] &&
         isspace(program_buffer[program_buffer_index])) {
    IncrementProgramBufferIndex();
  }
}

static bool IsString() {
  int length = 0;

  if ('"' != program_buffer[program_buffer_index]) {
    return false;
  }

  IncrementProgramBufferIndex();

  while (program_buffer[program_buffer_index] &&
         '"' != program_buffer[program_buffer_index]) {
    if (length < sizeof(token.value.text) - 1) {
      token.value.text[length++] = program_buffer[program_buffer_index];
    }

    IncrementProgramBufferIndex();
  }

  token.value.text[length] = '\0';

  if ('"' == program_buffer[program_buffer_index]) {
    IncrementProgramBufferIndex();
  }

  token.type = kTokenString;

  token.precedence = kPrecPrimary;

  return true;
}

static bool IsCharacter() {
  switch (program_buffer[program_buffer_index]) {
    case '=':
      IncrementProgramBufferIndex();

      if ('=' == program_buffer[program_buffer_index]) {
        token.type = kTokenEquals;
        token.precedence = kPrecComparison;

        break;
      }

      token.type = kTokenAssign;
      token.precedence = kPrecAssignment;

      DecrementProgramBufferIndex();

      break;
    case '!':
      IncrementProgramBufferIndex();

      if ('=' == program_buffer[program_buffer_index]) {
        token.type = kTokenNotEquals;
        token.precedence = kPrecComparison;

        break;
      }

      token.type = kTokenNot;
      token.precedence = kPrecUnary;

      DecrementProgramBufferIndex();

      break;
    case '(':
      token.type = kTokenLeftParenthesis;
      token.precedence = kPrecPrimary;

      break;
    case ')':
      token.type = kTokenRightParenthesis;
      token.precedence = kPrecPrimary;

      break;
    case '+':
      token.type = kTokenPlus;
      token.precedence = kPrecTerm;

      break;
    case '-':
      token.type = kTokenMinus;
      token.precedence = kPrecTerm;

      break;
    case '*':
      token.type = kTokenStar;
      token.precedence = kPrecFactor;

      break;
    case '/':
      token.type = kTokenSlash;
      token.precedence = kPrecFactor;

      break;
    case ':':
      token.type = kTokenColon;

      break;
    case '>':
      IncrementProgramBufferIndex();

      if ('=' == program_buffer[program_buffer_index]) {
        token.type = kTokenGreaterOrEquals;
        token.precedence = kPrecComparison;

        break;
      }

      token.type = kTokenGreaterThan;
      token.precedence = kPrecComparison;

      DecrementProgramBufferIndex();

      break;
    case '<':
      IncrementProgramBufferIndex();

      if ('=' == program_buffer[program_buffer_index]) {
        token.type = kTokenLessOrEquals;
        token.precedence = kPrecComparison;

        break;
      }

      token.type = kTokenLessThan;
      token.precedence = kPrecComparison;

      DecrementProgramBufferIndex();

      break;
    case ',':
      token.type = kTokenComma;
      token.precedence = kPrecPrimary;

      break;
    case '.':
      token.type = kTokenDot;
      token.precedence = kPrecPrimary;

      break;
    default:
      return false;
  }

  IncrementProgramBufferIndex();

  return true;
}

bool IsBinaryOperator(const TokenType token_type) {
  return 0 != (kTokenPlus == token_type || kTokenMinus == token_type ||
               kTokenStar == token_type || kTokenGreaterThan == token_type ||
               kTokenSlash == token_type || kTokenLessThan == token_type ||
               kTokenGreaterOrEquals == token_type ||
               kTokenLessOrEquals == token_type || kTokenEquals == token_type ||
               kTokenNotEquals == token_type);
}

static bool IsKeyword(const char* const buffer) {
  size_t keyword_index = 0;

  for (keyword_index = 0; keyword_index < kKeywordCount; keyword_index++) {
    const size_t kKeywordTextLength = strlen(kKeywordMap[keyword_index].kText);

    if (0 ==
        strncmp(buffer, kKeywordMap[keyword_index].kText, kKeywordTextLength)) {
      token.type = kKeywordMap[keyword_index].kType;

      return true;
    }
  }

  return false;
}

static bool IsBoolean(const char* const buffer) {
  if (0 == strncmp(buffer, "true", 4)) {
    token.type = kTokenBoolean;
    token.value.number = 1;
    token.precedence = kPrecPrimary;

    return true;
  }

  // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
  if (0 == strncmp(buffer, "false", 5)) {
    token.type = kTokenBoolean;
    token.value.number = 0;
    token.precedence = kPrecPrimary;

    return true;
  }

  return false;
}

static bool IsNumber() {
#ifdef __CC65__
  char* end = NULL;
  static const int kBase = 10;
#else
  char* end = nullptr;
  static constexpr int kBase = 10;
#endif
  size_t number_length = 0;

  if (!isdigit(program_buffer[program_buffer_index])) {
    return false;
  }

  token.type = kTokenNumber;
  token.value.number =
      (int)strtol(&program_buffer[program_buffer_index], &end, kBase);
  token.precedence = kPrecPrimary;

  number_length = end - &program_buffer[program_buffer_index];
  program_buffer_index += number_length;

  return true;
}

void ExtractIdentifierName(char* const buffer) {
  // size_t length = 0;
  // size_t start = token.start_of_token;

  // while (isalnum(program_buffer[start + length])) {
  //   length++;
  // }

  // size_t copy_length = length < max_length - 1 ? length : max_length - 1;

  size_t token_value_text_length = strlen(token.value.text);

  strncpy(buffer, token.value.text, token_value_text_length);

  buffer[token_value_text_length] = '\0';
}

void PeekNextToken() {
  const size_t kSavedProgramBufferIndex = program_buffer_index;
  Token saved_token = {};
  saved_token = token;

  ConsumeNextToken();

  next_token = token;

  program_buffer_index = kSavedProgramBufferIndex;
  token = saved_token;
}

void ConsumeNextToken() {
  SkipWhitespace();

  token.start_of_token = program_buffer_index;

  if ('\0' == program_buffer[program_buffer_index]) {
    token.type = kTokenEof;

    return;
  }

  if ((int)IsCharacter() || (int)IsString()) {
    return;
  }

  if (isalpha(program_buffer[program_buffer_index])) {
    int length = 0;
    char buffer[kTokenTextBufferSize];

    while (isalnum(program_buffer[program_buffer_index])) {
      if (length < (int)sizeof(buffer) - 1) {
        buffer[length++] = program_buffer[program_buffer_index];
      }

      IncrementProgramBufferIndex();
    }

    buffer[length] = '\0';

    if ((int)IsBoolean(buffer) || (int)IsKeyword(buffer)) {
      return;
    }

    token.type = kTokenIdentifier;
    token.precedence = kPrecPrimary;

    // NOLINTNEXTLINE(clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling)
    strncpy(token.value.text, buffer, strlen(buffer) + 1);

    return;
  }

  if (IsNumber()) {
    return;
  }

  printf("Error: Unexpected token '%c'.\n",
         program_buffer[program_buffer_index]);

  token.type = kTokenEof;
}
