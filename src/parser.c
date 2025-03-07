#include "parser.h"

#include <stdio.h>
#include <stdlib.h>

#include "lexer.h"
#include "vm.h"

static void ParseNumber();
static void ParseBoolean();

static void ParseExpression() {
  //Assuming boolean
  if (token.type != kTokenNumber) {
    ParseBoolean();
    return;
  }

  ParseNumber();

  ConsumeNextToken();

  while (kTokenPlus == token.type || kTokenMinus == token.type ||
         kTokenGreaterThan == token.type || kTokenLessThan == token.type) {
    const TokenType kOperation = token.type;

    ConsumeNextToken();

    switch (kOperation) {
      case kTokenPlus:
        ParseNumber();
        EmitByte(kOpAdd);
        break;
      case kTokenMinus:
        ParseNumber();
        EmitByte(kOpSubtract);
        break;
      case kTokenGreaterThan:
        ParseNumber();
        EmitByte(kOpGreaterThan);
        break;
      case kTokenLessThan:
        ParseNumber();
        EmitByte(kOpLessThan);
        break;
      default:
        printf("Expected Operator");
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
}

static void ParseBoolean() {
  if (kTokenTrue == token.type) {
    const size_t kIndex = AddBooleanConstant(true);
    EmitByte(kOpTrue);
  } else if (kTokenFalse == token.type) {
    const size_t kIndex = AddBooleanConstant(false);
    EmitByte(kOpFalse);
  }

}

static void ParseString() {
  if (kTokenQuotationMark != token.type) {
    return;
  }

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
