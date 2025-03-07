#include "parser.h"

#include <stdio.h>

#include "lexer.h"
#include "vm.h"

// static void ParseTerm();
static void ParseNumber();

static void ParseExpression() {
  ParseNumber();

  ConsumeNextToken();

  while (kTokenPlus == token.type || kTokenMinus == token.type) {
    const TokenType kOperation = token.type;

    ConsumeNextToken();

    ParseNumber();

    if (kTokenPlus == kOperation) {
      EmitByte(kOpAdd);
    }

    if (kTokenMinus == kOperation) {
      EmitByte(kOpSubtract);
    }
  }
}

// static void ParseTerm() {
//   ParseNumber();
//
//   while (kTokenStar == token.type || kTokenSlash == token.type) {
//     const TokenType kOperation = token.type;
//
//     ConsumeNextToken();
//
//     ParseNumber();
//
//     ConsumeNextToken();
//
//     if (kTokenStar == kOperation) {
//       EmitByte(kOpMultiply);
//     }
//
//     if (kTokenSlash == kOperation) {
//       EmitByte(kOpDivide);
//     }
//   }
// }

/**
 * Grammar:
 * factor -> number
 * | '(' expression ')'
 */
static void ParseNumber() {
  if (kTokenNumber == token.type) {
    const long kNumber = token.value;

    const size_t kIndex = AddNumberConstant(kNumber);

    EmitByte(kOpConstant);
    EmitByte(kIndex);
  }

  // ParseExpression();
}

static void ParseString() {
  const size_t kStringIndex = AddStringConstant(token.text);

  EmitByte(kOpConstant);
  EmitByte(kStringIndex);
}

/**
 * Grammar: print(expression)
 */
static void ParsePrintStatement() {
  ConsumeNextToken();

  if (kTokenLeftParenthesis != token.type) {
    printf("Expected '(' but got token type '%d'\n", (int)token.type);

    return;
  }

  ConsumeNextToken();

  if (kTokenQuotationMark != token.type) {
    ParseExpression();
  } else {
    ParseString();
  }

  ConsumeNextToken();

  if (kTokenRightParenthesis != token.type) {
    printf("Expected ')' but got token type '%d'\n", (int)token.type);

    return;
  }

  ConsumeNextToken();

  EmitByte(kOpPrint);
}

/**
 * Grammar: if(expression) statement* endif
 */
static void ParseIfStatement() {
  ConsumeNextToken();

  if (kTokenLeftParenthesis != token.type) {
    printf("Expected '(' but got token type '%d'\n", (int)token.type);

    return;
  }

  ConsumeNextToken();

  if (kTokenRightParenthesis != token.type) {
    printf("Expected ')' but got token type '%d'\n", (int)token.type);

    return;
  }

  ConsumeNextToken();

  EmitByte(kOpIf);
}

static void ParseStatement() {
  switch (token.type) {
    case kTokenPrint:
      ParsePrintStatement();

      break;
    case kTokenIf:
      ParseIfStatement();

      break;
    default:
      printf("Unregistered statement '%s'\n", token.text);

      token.type = kTokenEof;
  }
}

void ParseProgram(const char* const source_code_parameter) {
  source_code = source_code_parameter;
  ConsumeNextToken();

  while (kTokenEof != token.type) {
    ParseStatement();
  }
}
