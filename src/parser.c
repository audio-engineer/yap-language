#include "parser.h"

#include <stdio.h>
#ifdef __linux__
#include <stdlib.h>
#endif

#if defined(__CC65__) && !defined(NDEBUG)
#include "benchmark.h"
#endif
#include "lexer.h"
#include "vm.h"

/**
 * Grammar:
 * factor -> number
 * | '(' expression ')'
 */
static void ParseNumber() {
  const long kNumber = token.value.number;

  const size_t kIndex = AddNumberConstant(kNumber, kTypeNumber);

  EmitByte(kOpConstant);
  EmitByte(kIndex);
}

static void ParseBoolean() {
  size_t index = 0;

  if (!token.value.boolean) {
    index = AddNumberConstant(0, kTypeBoolean);
  } else {
    index = AddNumberConstant(1, kTypeBoolean);
  }

  EmitByte(kOpConstant);
  EmitByte(index);
}

static void ParseOperator(const TokenType operation) {
  switch (operation) {
    case kTokenPlus:
      EmitByte(kOpAdd);
      break;
    case kTokenMinus:
      EmitByte(kOpSubtract);
      break;
    case kTokenStar:
      EmitByte(kOpMultiply);
      break;
    case kTokenSlash:
      EmitByte(kOpDivide);
      break;
    case kTokenEquals:
      EmitByte(kOpEquals);
      break;
    case kTokenNotEquals:
      EmitByte(kOpNotEquals);
      break;
    case kTokenGreaterThan:
      EmitByte(kOpGreaterThan);
      break;
    case kTokenGreaterOrEquals:
      EmitByte(kOpGreaterOrEquals);
      break;
    case kTokenLessThan:
      EmitByte(kOpLessThan);
      break;
    case kTokenLessOrEquals:
      EmitByte(kOpLessOrEquals);
      break;
    default:
      printf("Undefined operator");
      token.type = kTokenEof;
  }
}

// NOLINTBEGIN(misc-no-recursion)
static void ParseNumericExpression(const Precedence precedence) {
  ParseNumber();

  ConsumeNextToken();

  while ((kTokenPlus == token.type || kTokenMinus == token.type ||
          kTokenStar == token.type || kTokenGreaterThan == token.type ||
          kTokenSlash == token.type || kTokenLessThan == token.type ||
          kTokenGreaterOrEquals == token.type ||
          kTokenLessOrEquals == token.type) &&
         precedence <= token.precedence) {
    const TokenType kOperation = token.type;
    const unsigned char kNextPrecedence = token.precedence + 1;

    ConsumeNextToken();

    ParseNumericExpression(kNextPrecedence);

    ParseOperator(kOperation);

    ConsumeNextToken();
  }
  source_code = token.start_of_token;
}
// NOLINTEND(misc-no-recursion)

static void ParseExpression() {
  switch (token.type) {
    case kTokenNumber:
      ParseNumericExpression(kPrecNone);
      break;
    case kTokenBoolean:
      ParseBoolean();
      break;
    default:
      printf("Could not parse expression");
      token.type = kTokenEof;
  }
}

static void ParseString() {
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
      printf("Unregistered statement '%s'\n", token.value.text);

      token.type = kTokenEof;
  }
}

void ParseProgram(const char* const source_code_parameter) {
  source_code = source_code_parameter;

#if defined(__CC65__) && !defined(NDEBUG)
  StartTimerA();
#endif

  ConsumeNextToken();

  while (kTokenEof != token.type) {
    ParseStatement();
  }

#if defined(__CC65__) && !defined(NDEBUG)
  StopTimerA();
#endif
}
