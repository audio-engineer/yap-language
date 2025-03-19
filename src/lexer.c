#include "lexer.h"

#include <ctype.h>
#ifdef __CC65__
#include <stdbool.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// NOLINTBEGIN(cppcoreguidelines-avoid-non-const-global-variables)
#ifdef __CC65__
const char* source_code = NULL;
#else
const char* source_code = nullptr;
#endif
Token token;
// NOLINTEND(cppcoreguidelines-avoid-non-const-global-variables)

typedef struct KeywordEntry {
  const char* const kText;
  const TokenType kType;
} KeywordEntry;

static const KeywordEntry kKeywordMap[] = {{"print", kTokenPrint},
                                           {"if", kTokenIf},
                                           {"endif", kTokenEndif},
                                           {"for", kTokenFor},
                                           {"endfor", kTokenEndfor}};

#ifdef __CC65__
static const size_t kKeywordCount = sizeof(kKeywordMap) / sizeof(KeywordEntry);
#else
static constexpr size_t kKeywordCount =
    sizeof(kKeywordMap) / sizeof(KeywordEntry);
#endif

static void SkipWhitespace() {
  while (*source_code && isspace(*source_code)) {
    source_code++;
  }
}

static bool IsString() {
  int length = 0;

  if ('"' != *source_code) {
    return false;
  }

  source_code++;

  while (*source_code && '"' != *source_code) {
    if (length < sizeof(token.value.text) - 1) {
      token.value.text[length++] = *source_code;
    }

    source_code++;
  }

  token.value.text[length] = '\0';

  if ('"' == *source_code) {
    source_code++;
  }

  token.type = kTokenString;

  return true;
}

static bool IsCharacter() {
  switch (*source_code) {
    case '=':
      token.type = kTokenEquals;
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
      source_code++;

      if ('=' == *source_code) {
        token.type = kTokenGreaterOrEquals;

        source_code++;

        return true;
      }

      token.type = kTokenGreaterThan;
      source_code--;

      break;
    case '<':
      source_code++;

      if ('=' == *source_code) {
        token.type = kTokenLessOrEquals;

        source_code++;

        return true;
      }

      token.type = kTokenLessThan;
      source_code--;

      break;
    default:
      return false;
  }

  token.value.text[0] = *source_code;
  token.value.text[1] = '\0';
  source_code++;

  return true;
}

static bool IsKeyword(const char* const buffer) {
  size_t keyword_index = 0;

  for (keyword_index = 0; keyword_index < kKeywordCount; keyword_index++) {
    if (0 == strncmp(buffer, kKeywordMap[keyword_index].kText,
                     strlen(kKeywordMap[keyword_index].kText))) {
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

  if (!isdigit(*source_code)) {
    return false;
  }

  token.type = kTokenNumber;
  token.value.number = strtol(source_code, &end, kBase);

  // Move source code to first character after the digit
  source_code = end;

  return true;
}

void ConsumeNextToken() {
  SkipWhitespace();

  if ('\0' == *source_code) {
    token.type = kTokenEof;

    return;
  }

  if ((int)IsCharacter() || (int)IsString()) {
    return;
  }

  if (isalpha(*source_code)) {
    int length = 0;
    char buffer[kTokenTextBufferSize];

    while (isalnum(*source_code)) {
      if (length < (int)sizeof(buffer) - 1) {
        buffer[length++] = *source_code;
      }

      source_code++;
    }

    buffer[length] = '\0';

    if ((int)IsBoolean(buffer) || (int)IsKeyword(buffer)) {
      return;
    }

    token.type = kTokenId;

    // NOLINTNEXTLINE(clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling)
    strncpy(token.value.text, buffer, strlen(buffer) + 1);

    return;
  }

  if (IsNumber()) {
    return;
  }

  printf("Unexpected token: '%c'\n", *source_code);

  token.type = kTokenEof;
}
