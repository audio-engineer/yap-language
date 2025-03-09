#include "parser.h"

#include <stdio.h>
#include <stdlib.h>

#include "lexer.h"
#include "vm.h"

static void ParseNumber();
static void ParseBoolean();

static void ParseExpression() {
  ParseNumber();
  ParseBoolean();

  ConsumeNextToken();

  while (kTokenPlus == token.type || kTokenMinus == token.type || kTokenGreaterThan == token.type ||kTokenLessThan == token.type) {
    const TokenType kOperation = token.type;

    ConsumeNextToken();

    if (ParseNumber()) {
      if (kTokenPlus == kOperation) {
        EmitByte(kOpAdd);
      }

      if (kTokenMinus == kOperation) {
        EmitByte(kOpSubtract);
      }

      if (kTokenGreaterThan == kOperation) {
        EmitByte(kOpGreaterThan);
      }

      if (kTokenLessThan == kOperation) {
        EmitByte(kOpLessThan);
      }
    } else if (ParseBoolean()) {
      if (kTokenGreaterThan == kOperation) {
        EmitByte(kOpGreaterThan);
      }

      if (kTokenLessThan == kOperation) {
        EmitByte(kOpLessThan);
      }
    } else {
      printf("Expected number or bool, also identifiers are not implemented");
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
static bool ParseNumber() {
  if (kTokenNumber == token.type) {
    const long kNumber = token.value.number;

    const size_t kIndex = AddNumberConstant(kNumber);

    EmitByte(kOpConstant);
    EmitByte(kIndex);
    return true;
  }
  return false;
}

static bool ParseBoolean() {
  if (kTokenTrue == token.type) {
    token.value.boolean = true;
    AddBooleanConstant(true);
    EmitByte(kOpTrue);
    return true;
  }

  if (kTokenFalse == token.type) {
    token.value.boolean = false;
    AddBooleanConstant(false);
    EmitByte(kOpFalse);
    return true;
  }

  return false;
}

static void ParseString() {
  if (kTokenQuotationMark != token.type) {
    return;
  }

  const size_t kStringIndex = AddStringConstant(token.value.text);

  EmitByte(kOpConstant);
  EmitByte(kStringIndex);
}

/**
 * Grammar: print(something)
 */
static void ParsePrintStatement() {
  ConsumeNextToken();

  if (kTokenLeftParenthesis != token.type) {
    printf("Expected '(' but got token type '%d'\n", (int)token.type);

    return;
  }

  ConsumeNextToken();

  if (kTokenQuotationMark == token.type) {
    ParseString();
  } else {
    ParseExpression();
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
      printf("Unregistered statement '%s'\n", token.value.text);
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
