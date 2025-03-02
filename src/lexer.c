#include "lexer.h"

#include <ctype.h>
#include <string.h>

// NOLINTBEGIN(cppcoreguidelines-avoid-non-const-global-variables)
const char* source_code = nullptr;
Token token;
// NOLINTEND(cppcoreguidelines-avoid-non-const-global-variables)

void SkipWhitespace() {
  while (*source_code && isspace(*source_code)) {
    source_code++;
  }
}

bool IsQuotationMark() {
  if ('"' != *source_code) {
    return false;
  }

  token.type = kQuotationMark;
  source_code++;

  int length = 0;

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

bool IsLeftParenthesis() {
  if ('(' != *source_code) {
    return false;
  }

  token.type = kLeftParenthesis;
  token.text[0] = *source_code;
  token.text[1] = '\0';

  source_code++;

  return true;
}

bool IsRightParenthesis() {
  if (')' != *source_code) {
    return false;
  }

  token.type = kRightParenthesis;
  token.text[0] = *source_code;
  token.text[1] = '\0';

  source_code++;

  return true;
}

static constexpr int kBufferSize = 100;

void ConsumeNextToken() {
  SkipWhitespace();

  if ('\0' == *source_code) {
    token.type = kEOF;

    return;
  }

  if ((int)IsQuotationMark() || (int)IsLeftParenthesis() ||
      (int)IsRightParenthesis()) {
    return;
  }

  if (isalpha(*source_code)) {
    int length = 0;
    char buffer[kBufferSize];

    while (isalnum(*source_code)) {
      if (length < (int)sizeof(buffer) - 1) {
        buffer[length++] = *source_code;
      }

      source_code++;
    }

    buffer[length] = '\0';

    // NOLINTNEXTLINE(clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling)
    strncpy(token.text, buffer, strlen(buffer));

    if (0 == strcmp(buffer, "print")) {
      token.type = kPrint;

      return;
    }

    if (0 == strcmp(buffer, "if")) {
      token.type = kIf;

      return;
    }

    return;
  }

  token.type = kEOF;
}
