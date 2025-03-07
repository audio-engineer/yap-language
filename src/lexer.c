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

static bool IsQuotationMark() {
  int length = 0;

  if ('"' != *source_code) {
    return false;
  }

  token.type = kTokenQuotationMark;
  source_code++;

  while (*source_code && '"' != *source_code) {
    if (length < sizeof(token.text) - 1) {
      token.text[length++] = *source_code;
    }

    source_code++;
  }

  token.text[length] = '\0';

  if ('"' == *source_code) {
    source_code++;
  }

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
    default:
      return false;
  }

  token.text[0] = *source_code;
  token.text[1] = '\0';
  source_code++;

  return true;
}

void ConsumeNextToken() {
  SkipWhitespace();

  if ('\0' == *source_code) {
    token.type = kTokenEof;

    return;
  }

  if ((int)IsCharacter() || (int)IsQuotationMark()) {
    return;
  }

  // Check if the token is a string
  if (isalpha(*source_code)) {
    size_t keyword_index = 0;
    int length = 0;
    char buffer[kTokenTextBufferSize];

    while (isalnum(*source_code)) {
      if (length < (int)sizeof(buffer) - 1) {
        buffer[length++] = *source_code;
      }

      source_code++;
    }

    buffer[length] = '\0';

    // NOLINTBEGIN(clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling)
    memset(token.text, 0, sizeof(token.text));
    strncpy(token.text, buffer, strlen(buffer));
    // NOLINTEND(clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling)

    for (keyword_index = 0; keyword_index < kKeywordCount; keyword_index++) {
      if (0 == strcmp(token.text, kKeywordMap[keyword_index].kText)) {
        token.type = kKeywordMap[keyword_index].kType;

        return;
      }
    }

    // If then token is not a keyword, it's an identifier
    token.type = kTokenId;

    return;
  }

  // Check if token is a number
  if (isdigit(*source_code)) {
#ifdef __CC65__
    char* end = NULL;
    static const int kBase = 10;
#else
    char* end = nullptr;
    static constexpr int kBase = 10;
#endif

    token.type = kTokenNumber;
    token.value = strtol(source_code, &end, kBase);

    // Move source code to first character after the digit
    source_code = end;

    return;
  }

  printf("Unexpected token: '%c'\n", *source_code);

  token.type = kTokenEof;
}
