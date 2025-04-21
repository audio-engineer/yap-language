#include "lexer.h"

#include <ctype.h>
#include <stdarg.h>
#ifdef __CC65__
#include <stdbool.h>
#endif
#if defined(__CC65__) || defined(__linux__)
#include <stddef.h>
#endif
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
  // NOLINTNEXTLINE(clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling)
  memset(program_buffer, 0, kProgramBufferSize);

  program_buffer_index = 0;
}

static void IncrementProgramBufferIndex() {
  if (kProgramBufferSize < program_buffer_index) {
    puts("Error: Program buffer overflow.");

    return;
  }

  ++program_buffer_index;
}

static void DecrementProgramBufferIndex() {
  if (0 == program_buffer_index) {
    puts("Error: Program buffer underflow.");

    return;
  }

  --program_buffer_index;
}

bool __cdecl__ AcceptTokenImplementation(const size_t token_type_list_length,
                                         ...) {
#ifdef __CC65__
  // NOLINTNEXTLINE(cppcoreguidelines-init-variables)
  va_list args;
#else
  // cppcheck-suppress va_list_usedBeforeStarted
  va_list args = {};
#endif

  size_t index = 0;

  va_start(args, token_type_list_length);

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

  return true;
}

static bool IsCharacter() {
  switch (program_buffer[program_buffer_index]) {
    case '=':
      IncrementProgramBufferIndex();

      if ('=' == program_buffer[program_buffer_index]) {
        token.type = kTokenEquals;

        break;
      }

      token.type = kTokenAssign;

      DecrementProgramBufferIndex();

      break;
    case '!':
      IncrementProgramBufferIndex();

      if ('=' == program_buffer[program_buffer_index]) {
        token.type = kTokenNotEquals;

        break;
      }

      token.type = kTokenNot;

      DecrementProgramBufferIndex();

      break;
    case '(':
      token.type = kTokenLeftParenthesis;

      break;
    case ')':
      token.type = kTokenRightParenthesis;

      break;
    case '+':
      token.type = kTokenPlus;

      break;
    case '-':
      token.type = kTokenMinus;

      break;
    case '*':
      token.type = kTokenStar;

      break;
    case '/':
      token.type = kTokenSlash;

      break;
    case ':':
      token.type = kTokenColon;

      break;
    case '>':
      IncrementProgramBufferIndex();

      if ('=' == program_buffer[program_buffer_index]) {
        token.type = kTokenGreaterOrEquals;

        break;
      }

      token.type = kTokenGreaterThan;

      DecrementProgramBufferIndex();

      break;
    case '<':
      IncrementProgramBufferIndex();

      if ('=' == program_buffer[program_buffer_index]) {
        token.type = kTokenLessOrEquals;

        break;
      }

      token.type = kTokenLessThan;

      DecrementProgramBufferIndex();

      break;
    case ',':
      token.type = kTokenComma;

      break;
    case '.':
      token.type = kTokenDot;

      break;
    default:
      return false;
  }

  IncrementProgramBufferIndex();

  return true;
}

static bool IsKeyword(const char* const buffer) {
  size_t keyword_index = 0;

  for (keyword_index = 0; keyword_index < kKeywordCount; ++keyword_index) {
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

    return true;
  }

  // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
  if (0 == strncmp(buffer, "false", 5)) {
    token.type = kTokenBoolean;
    token.value.number = 0;

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

  number_length = end - &program_buffer[program_buffer_index];
  program_buffer_index += number_length;

  return true;
}

void ExtractIdentifierName(char* const buffer) {
  size_t token_value_text_length = strlen(token.value.text);

  // NOLINTNEXTLINE(clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling)
  strncpy(buffer, token.value.text, token_value_text_length);

  buffer[token_value_text_length] = '\0';
}

void PeekNextToken() {
  const size_t kSavedProgramBufferIndex = program_buffer_index;
  Token saved_token = {};

  // cppcheck-suppress redundantInitialization
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
