#include "parser.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lexer.h"

char* ParseString() {
  if (kQuotationMark != token.type) {
    return nullptr;
  }

  char* const kValue = calloc(100, sizeof(char));
  // NOLINTNEXTLINE(clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling)
  strncpy(kValue, token.text, strlen(token.text));

  return kValue;
}

void ParseStatement() {
  if (kPrint == token.type) {
    ConsumeNextToken();

    if (kLeftParenthesis == token.type) {
      ConsumeNextToken();

      char* const kValue = ParseString();
      ConsumeNextToken();

      if (kRightParenthesis == token.type) {
        ConsumeNextToken();
      } else {
        free(kValue);

        printf("Expected ')' but got token type '%d'\n", (int)token.type);

        return;
      }

      printf("%s\n", kValue);

      free(kValue);

      return;
    }

    printf("Expected '(' but got token type '%d'\n", (int)token.type);

    return;
  }

  printf("Unregistered statement '%s'\n", token.text);
}

void ParseProgram(const char* const source_code_parameter) {
  source_code = source_code_parameter;
  ConsumeNextToken();

  while (kEOF != token.type) {
    ParseStatement();
  }
}
