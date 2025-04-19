#ifdef __CC65__
#include <stdbool.h>
#endif
#include <stdio.h>
#ifdef __linux__
#include <stdlib.h>
#endif
#include <string.h>

#if defined(__CC65__) && !defined(NDEBUG)
#include "benchmark.h"
#endif
#include "lexer.h"
#include "parser.h"
#include "vm.h"

#ifdef __CC65__
enum { kSymbolNameSize = 16, kSymbolTableSize = 16 };
#else
static constexpr int kSymbolNameSize = 16;
static constexpr int kSymbolTableSize = 16;
#endif

typedef struct SymbolTableEntry {
  char name[kSymbolNameSize];
  VariableType type;
  size_t index;
} SymbolTableEntry;

// NOLINTBEGIN(cppcoreguidelines-avoid-non-const-global-variables)
static SymbolTableEntry symbol_table[kSymbolTableSize];
static size_t symbol_table_index = 0;

static char local_names[16][kSymbolNameSize];
static size_t local_count = 0;

static bool is_function_scope = false;
// NOLINTEND(cppcoreguidelines-avoid-non-const-global-variables)

static void ParseStatement();

static void ParseExpression();

static void ParseArithmeticExpression();

static void ParseIdentifierStatement();

static void ParseFunctionDefinition(const char* identifier_name,
                                    VariableType return_type);

void ResetParserState() {
  global_variable_index = 0;
  symbol_table_index = 0;
  local_count = 0;

  memset(symbol_table, 0, kSymbolTableSize);
  memset(local_names, 0, kSymbolNameSize);
}

// static void TokenTypeAssertionError(const char* const expected,
//                                     const TokenType actual) {
//   printf("Error: Expected '%s' but got '%d'.\n", expected, actual);
// }

static VariableType TokenTypeToVariableType(const TokenType token_type) {
  switch (token_type) {
    case kTokenInt:
      return kVariableTypeInt;
    case kTokenStr:
      return kVariableTypeStr;
    case kTokenBool:
      return kVariableTypeBool;
    case kTokenFloat:
      return kVariableTypeFloat;
    default:
      puts("Error: Unknown variable type.");

      return kVariableTypeInt;
  }
}

static size_t FindGlobalSymbol(const char* const identifier_name) {
  size_t index = 0;

  for (index = 0; index <= symbol_table_index; index++) {
    if (0 == strcmp(symbol_table[index].name, identifier_name)) {
      return index;
    }
  }

  return (size_t)-1;
}

static size_t FindLocalSymbol(const char* const identifier_name) {
  size_t index = 0;

  for (index = 0; index < local_count; index++) {
    if (0 == strcmp(local_names[index], identifier_name)) {
      return index;
    }
  }

  return (size_t)-1;
}

static size_t AddSymbol(const char* const identifier_name,
                        const VariableType var_type) {
  if (kSymbolNameSize < strlen(identifier_name)) {
    puts("Error: Symbol name is too long.");

    return (size_t)-1;
  }

  if (symbol_table_index >= kSymbolTableSize) {
    puts("Error: Too many global variables.");

    return (size_t)-1;
  }

  if ((size_t)-1 != FindGlobalSymbol(identifier_name)) {
    printf("Error: Symbol '%s' already defined.\n", identifier_name);

    return (size_t)-1;
  }

  strncpy(symbol_table[global_variable_index].name, identifier_name,
          sizeof(symbol_table[global_variable_index].name) - 1);
  symbol_table[global_variable_index].index = symbol_table_index;
  symbol_table[global_variable_index].type = var_type;

  return global_variable_index;
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
      EmitByte(kOpGreaterThanOrEqualTo);
      break;
    case kTokenLessThan:
      EmitByte(kOpLessThan);
      break;
    case kTokenLessOrEquals:
      EmitByte(kOpLessThanOrEqualTo);
      break;
    default:
      puts("Error: Undefined operator.");
      token.type = kTokenEof;
  }
}

static void ParseNumber() {
  const int kNumber = token.value.number;
  const size_t kIndex = AddNumberConstant(kNumber, kConstantTypeNumber);

  EmitByte(kOpConstant);
  EmitByte(kIndex);
}

static void ParseBoolean() {
  size_t index = 0;

  if (0 == token.value.number) {
    index = AddNumberConstant(0, kConstantTypeBoolean);
  } else {
    index = AddNumberConstant(1, kConstantTypeBoolean);
  }

  EmitByte(kOpConstant);
  EmitByte(index);
}

// NOLINTNEXTLINE(misc-no-recursion)
static void ParseFunctionCall() {
  size_t arity = 0;

  while (token.type != kTokenRightParenthesis && token.type != kTokenEof) {
    ParseExpression();
    ++arity;

    if (AcceptToken(1, kTokenRightParenthesis)) {
      break;
    }

    if (AcceptToken(1, kTokenComma)) {
      continue;
    }

    if (!((int)AcceptToken(1, kTokenComma) &&
          (int)ExpectToken(1, kTokenRightParenthesis))) {
      return;
    }
  }

  EmitByte(kOpCallFunction);
  EmitByte(arity);
}

// NOLINTNEXTLINE(misc-no-recursion)
static void ParseIdentifier(const char* const identifier_name) {
  size_t index = 0;

  index = FindLocalSymbol(identifier_name);

  if ((size_t)-1 != index) {
    EmitByte(kOpLoadLocal);
    EmitByte((unsigned char)index);

    // TODO(Martin): Implement type checker.
    ++instruction_index;

    return;
  }

  index = FindGlobalSymbol(identifier_name);

  if ((size_t)-1 == index) {
    printf("Error: Symbol '%s' is not defined.\n", identifier_name);

    return;
  }

  EmitByte(kOpLoadGlobal);
  EmitByte((unsigned char)index);

  // TODO(Martin): Implement type checker.
  ++instruction_index;

  if (AcceptToken(1, kTokenLeftParenthesis)) {
    ParseFunctionCall();
  }
}

static void ParseString() {
  const size_t kStringIndex = AddStringConstant(token.value.text);

  EmitByte(kOpConstant);
  EmitByte(kStringIndex);
}

// NOLINTNEXTLINE(misc-no-recursion)
static void ParseFactor() {
  if (AcceptToken(1, kTokenIdentifier)) {
    ParseIdentifier(token.value.text);
  }

  if (AcceptToken(1, kTokenNumber)) {
    ParseNumber();
  }

  if (AcceptToken(1, kTokenBoolean)) {
    ParseBoolean();
  }

  if (AcceptToken(1, kTokenString)) {
    ParseString();
  }

  if (AcceptToken(1, kTokenLeftParenthesis)) {
    ParseArithmeticExpression();
    ExpectToken(1, kTokenRightParenthesis);
  }
}

// NOLINTNEXTLINE(misc-no-recursion)
static void ParseTerm() {
  ParseFactor();

  while (kTokenStar == token.type || kTokenSlash == token.type) {
    const TokenType kOperator = token.type;

    ConsumeNextToken();
    ParseFactor();
    ParseOperator(kOperator);
  }
}

// NOLINTNEXTLINE(misc-no-recursion)
static void ParseArithmeticExpression() {
  if (kTokenPlus == token.type || kTokenMinus == token.type) {
    const TokenType kOperator = token.type;

    ConsumeNextToken();
    ParseOperator(kOperator);
  }

  ParseTerm();

  while (kTokenPlus == token.type || kTokenMinus == token.type) {
    const TokenType kOperator = token.type;

    ConsumeNextToken();
    ParseTerm();
    ParseOperator(kOperator);
  }
}

// NOLINTNEXTLINE(misc-no-recursion)
static void ParseRelationalExpression() {
  if (AcceptToken(1, kTokenNot)) {
    const TokenType kOperator = token.type;

    ParseArithmeticExpression();
    ParseOperator(kOperator);

    return;
  }

  ParseArithmeticExpression();

  if (kTokenEquals == token.type || kTokenNotEquals == token.type ||
      kTokenGreaterThan == token.type || kTokenGreaterOrEquals == token.type ||
      kTokenLessThan == token.type || kTokenLessOrEquals == token.type) {
    const TokenType kOperator = token.type;

    ConsumeNextToken();
    ParseArithmeticExpression();
    ParseOperator(kOperator);
  }
}

// NOLINTNEXTLINE(misc-no-recursion)
static void ParseLogicalExpression() { ParseRelationalExpression(); }

// NOLINTNEXTLINE(misc-no-recursion)
static void ParseExpression() { ParseLogicalExpression(); }

// NOLINTNEXTLINE(misc-no-recursion)
static void ParsePrintStatement() {
  if (!ExpectToken(1, kTokenPrint)) {
    return;
  }

  if (!ExpectToken(1, kTokenLeftParenthesis)) {
    return;
  }

  ParseExpression();

  if (!ExpectToken(1, kTokenRightParenthesis)) {
    return;
  }

  EmitByte(kOpPrint);
}

// NOLINTNEXTLINE(misc-no-recursion)
static void ParseIfStatement() {
  size_t if_jump_address_index = 0;

  if (!ExpectToken(1, kTokenIf)) {
    return;
  }

  if (!ExpectToken(1, kTokenLeftParenthesis)) {
    return;
  }

  ParseExpression();

  if (!ExpectToken(1, kTokenRightParenthesis)) {
    return;
  }

  EmitByte(kOpJumpIfFalse);

  if_jump_address_index = instruction_index;

  EmitByte(0);

  while (kTokenEndif != token.type && kTokenElse != token.type &&
         kTokenEof != token.type) {
    ParseStatement();
  }

  if (kTokenElse == token.type) {
    size_t else_jump_address_index = 0;

    EmitByte(kOpJump);

    else_jump_address_index = instruction_index;

    EmitByte(0);

    instructions[if_jump_address_index] = instruction_index;

    ConsumeNextToken();

    while (kTokenEndif != token.type && kTokenEof != token.type) {
      ParseStatement();
    }

    instructions[else_jump_address_index] = instruction_index;
  } else {
    instructions[if_jump_address_index] = instruction_index;
  }

  ExpectToken(1, kTokenEndif);
}

static void DefineVariable(const char* const identifier_name,
                           const VariableType type, const bool is_local) {
  size_t symbol_index = 0;

  // const size_t kNameIndex = AddIdentifierConstant(identifier_name);

  if ((int)is_local && !is_function_scope) {
    printf("Error: Cannot define local variable outside function scope.\n");

    token.type = kTokenEof;

    return;
  }

  // if (is_local) {
  //   EmitByte(kOpDefineLocal);
  //   EmitByte(local_count++);
  //
  //   return;
  // }

  symbol_index = AddSymbol(identifier_name, type);
  ++global_variable_index;

  EmitByte(kOpStoreGlobal);
  EmitByte(symbol_index);
  EmitByte(type);
}

static void SetVariable(const char* const identifier_name) {
  size_t index = 0;

  index = FindLocalSymbol(identifier_name);

  if (((size_t)-1 != index) && is_function_scope) {
    EmitByte(kOpStoreLocal);
    EmitByte((unsigned char)index);

    // TODO(Martin): Implement type checker.
    ++instruction_index;

    return;
  }

  index = FindGlobalSymbol(identifier_name);

  if ((size_t)-1 == index) {
    puts("Error: Symbol not found.");

    return;
  }

  EmitByte(kOpStoreGlobal);
  EmitByte((unsigned char)index);

  // TODO(Martin): Implement type checker.
  ++instruction_index;
}

// NOLINTNEXTLINE(misc-no-recursion)
static void ParseVariableDeclaration(const char* const identifier_name) {
  // bool is_local = false;
  VariableType variable_type = 0;

  if (!ExpectToken(1, kTokenColon)) {
    return;
  }

  variable_type = TokenTypeToVariableType(token.type);

  if (!ExpectToken(4, kTokenInt, kTokenStr, kTokenBool, kTokenFloat)) {
    return;
  }

  if (!ExpectToken(1, kTokenAssign)) {
    return;
  }

  if (AcceptToken(1, kTokenFunc)) {
    ParseFunctionDefinition(identifier_name, variable_type);

    return;
  }

  ParseExpression();

  DefineVariable(identifier_name, variable_type, false);
}

static void ParseVariableAssignment(const char* const identifier_name) {
  ConsumeNextToken();
  ParseExpression();
  SetVariable(identifier_name);
}

// NOLINTNEXTLINE(misc-no-recursion, readability-function-cognitive-complexity)
static void ParseFunctionDefinition(const char* const identifier_name,
                                    const VariableType return_type) {
  size_t symbol_index = 0;
  size_t arity = 0;
  size_t arity_index = 0;
  size_t if_jump_address_index = 0;
  size_t body_start = 0;

  if (!ExpectToken(1, kTokenLeftParenthesis)) {
    return;
  }

  is_function_scope = true;

  // Parse parameters
  while (token.type != kTokenRightParenthesis && token.type != kTokenEof) {
    char parameter_identifier_name[32];
    VariableType parameter_type = 0;

    ExtractIdentifierName(parameter_identifier_name);

    if (!ExpectToken(1, kTokenIdentifier)) {
      return;
    }

    if (!ExpectToken(1, kTokenColon)) {
      return;
    }

    // TODO(Martin): Implement type checker.
    // NOLINTNEXTLINE(clang-analyzer-deadcode.DeadStores)
    parameter_type = TokenTypeToVariableType(token.type);

    if (!ExpectToken(4, kTokenInt, kTokenStr, kTokenBool, kTokenFloat)) {
      return;
    }

    strncpy(local_names[arity], parameter_identifier_name,
            sizeof(local_names[0]));
    ++arity;
    ++local_count;

    if (AcceptToken(1, kTokenRightParenthesis)) {
      break;
    }

    if (AcceptToken(1, kTokenComma)) {
      continue;
    }

    if (!((int)AcceptToken(1, kTokenComma) &&
          (int)ExpectToken(1, kTokenIdentifier))) {
      return;
    }
  }

  if_jump_address_index = instruction_index;
  EmitByte(kOpJump);
  EmitByte(0);

  body_start = instruction_index;
  EmitByte(kOpPushCallFrame);
  EmitByte(arity);
  EmitByte(return_type);

  for (arity_index = 0; arity_index < arity; ++arity_index) {
    EmitByte(kOpStoreLocal);
    EmitByte((unsigned char)arity_index);

    // TODO(Martin): Implement type checker.
    ++instruction_index;
  }

  while (kTokenEndfunc != token.type && kTokenEof != token.type) {
    ParseStatement();
  }

  if (!ExpectToken(1, kTokenEndfunc)) {
    return;
  }

  EmitByte(kOpReturn);

  //
  instructions[if_jump_address_index + 1] = instruction_index;

  symbol_index = AddSymbol(identifier_name, return_type);
  ++global_variable_index;

  EmitByte(kOpDefineFunction);
  EmitByte(symbol_index);
  EmitByte(body_start);
  EmitByte(arity);
  EmitByte(return_type);
}

static void ParseReturnStatement() {
  if (!ExpectToken(1, kTokenRet)) {
    return;
  }

  ParseExpression();
  EmitByte(kOpReturn);
}

// NOLINTNEXTLINE(misc-no-recursion)
static void ParseIdentifierStatement() {
  char identifier_name[32];

  ExtractIdentifierName(identifier_name);

  if (!ExpectToken(1, kTokenIdentifier)) {
    return;
  }

  switch (token.type) {
    case kTokenColon:
      ParseVariableDeclaration(identifier_name);

      break;
    case kTokenAssign:
      ParseVariableAssignment(identifier_name);

      break;
    default:
      puts("Error: Unexpected token after identifier.");

      token.type = kTokenEof;
  }
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
    case kTokenIdentifier: {
      PeekNextToken();

      if (kTokenColon == next_token.type ||
          kTokenLeftParenthesis == next_token.type ||
          kTokenAssign == next_token.type) {
        ParseIdentifierStatement();

        break;
      }

      ParseExpression();

      break;
    }
    case kTokenLocal:
      // ParseVariableDeclaration(name);

      break;
    case kTokenRet:
      ParseReturnStatement();

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

  ResetParserState();

#if defined(__CC65__) && !defined(NDEBUG)
  StopTimerA();
#endif
}
