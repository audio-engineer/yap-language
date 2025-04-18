#include <stdio.h>
#ifdef __linux__
#include <stdlib.h>
#endif

#if defined(__CC65__) && !defined(NDEBUG)
#include "benchmark.h"
#endif
#include "lexer.h"
#include "parser.h"
#include "vm.h"

static void ParseStatement();

static void TokenTypeAssertionError(const char* const expected,
                                    const TokenType actual) {
  printf("Error: Expected '%s' but got '%d'.\n", expected, actual);
}

/**
 * Grammar:
 * factor -> number
 * | '(' expression ')'
 */
static void ParseNumber() {
  const int kNumber = token.value.number;

  const size_t kIndex = AddNumberConstant(kNumber, kTypeNumber);

  EmitByte(kOpConstant);
  EmitByte(kIndex);
}

static void ParseBoolean() {
  size_t index = 0;

  if (0 == token.value.number) {
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
      puts("Error: Undefined operator.");
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
          kTokenLessOrEquals == token.type || kTokenEquals == token.type ||
          kTokenNotEquals == token.type) &&
         precedence <= token.precedence) {
    const TokenType kOperation = token.type;
    const Precedence kNextPrecedence = token.precedence + 1;

    ConsumeNextToken();

    ParseNumericExpression(kNextPrecedence);

    ParseOperator(kOperation);

    ConsumeNextToken();
  }
  program_buffer_index = token.start_of_token;
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
      puts("Error: Could not parse expression.");
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
    TokenTypeAssertionError("(", token.type);

    return;
  }

  ConsumeNextToken();

  if (kTokenString != token.type) {
    ParseExpression();
  } else {
    ParseString();
  }

  ConsumeNextToken();

  if (kTokenRightParenthesis != token.type) {
    TokenTypeAssertionError(")", token.type);

    return;
  }

  ConsumeNextToken();

  EmitByte(kOpPrint);
}

/**
 * Grammar: if(expression) statement* endif
 */
// NOLINTNEXTLINE(misc-no-recursion)
static void ParseIfStatement() {
  size_t if_jump_address_index = 0;
  ConsumeNextToken();

  if (kTokenLeftParenthesis != token.type) {
    TokenTypeAssertionError("(", token.type);

    return;
  }
  ConsumeNextToken();
  ParseExpression();
  ConsumeNextToken();

  if (kTokenRightParenthesis != token.type) {
    TokenTypeAssertionError(")", token.type);

    return;
  }

  EmitByte(kOpJumpIfFalse);
  if_jump_address_index = instruction_index;
  EmitByte(0);

  ConsumeNextToken();

  while (token.type != kTokenEndif && token.type != kTokenElse &&
         token.type != kTokenEof) {
    ParseStatement();
  }

  if (token.type == kTokenElse) {
    size_t else_jump_address_index = 0;
    EmitByte(kOpJump);
    else_jump_address_index = instruction_index;
    EmitByte(0);

    instructions[if_jump_address_index] = instruction_index;

    ConsumeNextToken();

    while (token.type != kTokenEndif && token.type != kTokenEof) {
      ParseStatement();
    }

    instructions[else_jump_address_index] = instruction_index;
  } else {
    instructions[if_jump_address_index] = instruction_index;
  }

  if (token.type != kTokenEndif) {
    TokenTypeAssertionError("endif", token.type);
  }
  ConsumeNextToken();
}

// NOLINTNEXTLINE(misc-no-recursion)
static void ParseStatement() {
  switch (token.type) {
    case kTokenPrint:
      ParsePrintStatement();

      break;
    case kTokenIf:
      ParseIfStatement();

      break;
    default:
      printf("Error: Unregistered statement '%s'.\n", token.value.text);

      token.type = kTokenEof;
  }
}

void ParseProgram() {
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
