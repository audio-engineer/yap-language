#include "parser.h"

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
#include "vm.h"

#ifdef __CC65__
enum { kIdentifierNameLength = 16, kSymbolTableSize = 16 };
#else
static constexpr int kIdentifierNameLength = 16;
static constexpr int kSymbolTableSize = 16;
#endif

typedef struct SymbolTableEntry {
  char name[kIdentifierNameLength];
  VariableType type;
  size_t index;
} SymbolTableEntry;

const char* VariableTypeToString(VariableType type) {
  switch (type) {
    case kVariableTypeInt:
      return "int";
    case kVariableTypeStr:
      return "str";
    case kVariableTypeBool:
      return "bool";
    case kVariableTypeFloat:
      return "float";
    default:
      return "unknown";
  }
}

// NOLINTBEGIN(cppcoreguidelines-avoid-non-const-global-variables)
static SymbolTableEntry symbol_table[kSymbolTableSize];

static char local_names[kSymbolTableSize][kIdentifierNameLength];
static size_t local_count = 0;

static bool is_function_scope = false;
// NOLINTEND(cppcoreguidelines-avoid-non-const-global-variables)

static void ParseStatement();

static VariableType ParseExpression();

static VariableType ParseArithmeticExpression();

static void ParseIdentifierStatement(const char* identifier_name);

static void ParseFunctionDefinition(const char* identifier_name,
                                    VariableType return_type);

void ResetParserState() {
  // NOLINTBEGIN(clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling)
  memset(symbol_table, 0, kSymbolTableSize * sizeof(SymbolTableEntry));
  memset(local_names, 0, kSymbolTableSize);
  // NOLINTEND(clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling)

  global_variable_index = 0;
  local_count = 0;
}

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
    case kTokenArray:
      return kVariableTypeArray;
    default:
      puts("Error: Unknown variable type.");
      return kVariableTypeUnknown;
  }
}

static size_t FindGlobalSymbol(const char* const identifier_name) {
  size_t index = 0;

  for (index = 0; index <= global_variable_index; ++index) {
    if (0 == strcmp(symbol_table[index].name, identifier_name)) {
      return index;
    }
  }

  return (size_t)-1;
}

static size_t FindLocalSymbol(const char* const identifier_name) {
  size_t index = 0;

  for (index = 0; index < local_count; ++index) {
    if (0 == strcmp(local_names[index], identifier_name)) {
      return index;
    }
  }

  return (size_t)-1;
}

static size_t AddSymbol(const char* const identifier_name,
                        const VariableType var_type) {
  if (kIdentifierNameLength < strlen(identifier_name)) {
    puts("Error: Symbol name is too long.");

    return (size_t)-1;
  }

  if (global_variable_index >= kSymbolTableSize) {
    puts("Error: Too many global variables.");

    return (size_t)-1;
  }

  if ((size_t)-1 != FindGlobalSymbol(identifier_name)) {
    printf("Error: Already defined symbol '%s'.\n", identifier_name);

    return (size_t)-1;
  }

  // NOLINTNEXTLINE(clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling)
  strncpy(symbol_table[global_variable_index].name, identifier_name,
          sizeof(symbol_table[global_variable_index].name) - 1);
  symbol_table[global_variable_index].index = global_variable_index;
  symbol_table[global_variable_index].type = var_type;

  return global_variable_index++;
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
    case kTokenPercent:
      EmitByte(kOpModulo);

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

static VariableType ParseNumber(const int number) {
  const size_t kIndex = AddNumberConstant(number, kConstantTypeNumber);

  EmitByte(kOpConstant);
  EmitByte(kIndex);
  return kVariableTypeInt;
}

static VariableType ParseBoolean(const int boolean_value) {
  const size_t kIndex = AddNumberConstant(boolean_value, kConstantTypeBoolean);

  EmitByte(kOpConstant);
  EmitByte(kIndex);
  return kVariableTypeBool;
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

    if (!(AcceptToken(1, kTokenComma) &&
          ExpectToken(1, kTokenRightParenthesis))) {
      return;
    }
  }

  EmitByte(kOpCallFunction);
  EmitByte(arity);
}

// NOLINTNEXTLINE(misc-no-recursion)
static VariableType ParseIdentifier(const char* const identifier_name) {
  size_t index = 0;
  VariableType var_type = kVariableTypeUnknown;

  index = FindLocalSymbol(identifier_name);

  if ((size_t)-1 != index) {
    EmitByte(kOpLoadLocal);
    EmitByte((unsigned char)index);
    ++instruction_address;
    var_type = kVariableTypeInt;
  } else {
    index = FindGlobalSymbol(identifier_name);
    if (index == (size_t)-1) {
      printf("Error: Undefined symbol '%s'.\n", identifier_name);
      return kVariableTypeUnknown;
    }

    EmitByte(kOpLoadGlobal);
    EmitByte((unsigned char)index);
    ++instruction_address;

    var_type = symbol_table[index].type;
  }
  if (AcceptToken(1, kTokenLeftBracket)) {
    VariableType index_type = ParseExpression();

    if (index_type != kVariableTypeInt) {
      puts("Type error: Array index must be integer.");
      token.type = kTokenEof;
      return kVariableTypeUnknown;
    }

    if (!ExpectToken(1, kTokenRightBracket)) {
      return kVariableTypeUnknown;
    }

    if (AcceptToken(1, kTokenAssign)) {
      VariableType value_type = ParseExpression();

      if (value_type != kVariableTypeInt) {
        puts("Type error: Only integer values can be assigned to arrays.");
        token.type = kTokenEof;
        return kVariableTypeUnknown;
      }

      EmitByte(kOpStoreElement);
      EmitByte((unsigned char)index);
      return kVariableTypeInt;
    }
    EmitByte(kOpIndexArray);
    return kVariableTypeInt;
  }

  if (AcceptToken(1, kTokenLeftParenthesis)) {
    ParseFunctionCall();
    return kVariableTypeUnknown;
  }
  return var_type;
}

static VariableType ParseString(const char* const string) {
  const size_t kStringIndex = AddStringConstant(string);

  EmitByte(kOpConstant);
  EmitByte(kStringIndex);
  return kVariableTypeStr;
}

// NOLINTNEXTLINE(misc-no-recursion)
static VariableType ParseFactor() {
  Token saved_token = {};

  // cppcheck-suppress redundantInitialization
  saved_token = token;

  if (AcceptToken(1, kTokenIdentifier)) {
    return ParseIdentifier(saved_token.value.text);
  }

  if (AcceptToken(1, kTokenNumber)) {
    return ParseNumber(saved_token.value.number);
  }

  if (AcceptToken(1, kTokenBoolean)) {
    return ParseBoolean(saved_token.value.number);
  }

  if (AcceptToken(1, kTokenString)) {
    return ParseString(saved_token.value.text);
  }

  if (AcceptToken(1, kTokenLeftParenthesis)) {
    VariableType type = ParseExpression();
    ExpectToken(1, kTokenRightParenthesis);
    return type;
  }
  puts("Error: Invalid expression.");
  token.type = kTokenEof;
  return kVariableTypeInt;
}

// NOLINTNEXTLINE(misc-no-recursion)
static VariableType ParseTerm() {
  VariableType left_type = ParseFactor();
  VariableType right_type = kVariableTypeUnknown;

  while (kTokenStar == token.type || kTokenSlash == token.type ||
         kTokenPercent == token.type) {
    const TokenType kOperator = token.type;

    ConsumeNextToken();
    right_type = ParseFactor();
    if (left_type != kVariableTypeInt || right_type != kVariableTypeInt) {
      puts("Type error: Arithmetic operands must be integers.");
      token.type = kTokenEof;
    }

    ParseOperator(kOperator);
    left_type = kVariableTypeInt;
  }
  return left_type;
}

// NOLINTNEXTLINE(misc-no-recursion)
static VariableType ParseArithmeticExpression() {
  VariableType left_type = kVariableTypeUnknown;
  VariableType right_type = kVariableTypeUnknown;

  if (kTokenPlus == token.type || kTokenMinus == token.type) {
    const TokenType kOperator = token.type;

    ConsumeNextToken();
    left_type = ParseTerm();
    if (left_type != kVariableTypeInt) {
      puts("Type error: Unary operator requires integer.");
      token.type = kTokenEof;
    }

    ParseOperator(kOperator);
    return kVariableTypeInt;
  }

  left_type = ParseTerm();

  while (kTokenPlus == token.type || kTokenMinus == token.type) {
    const TokenType kOperator = token.type;

    ConsumeNextToken();
    right_type = ParseTerm();
    if (left_type != kVariableTypeInt || right_type != kVariableTypeInt) {
      puts("Type error: Arithmetic operands must be integers.");
      token.type = kTokenEof;
    }

    ParseOperator(kOperator);
    left_type = kVariableTypeInt;
  }
  return left_type;
}

// NOLINTNEXTLINE(misc-no-recursion)
static VariableType ParseRelationalExpression() {
  VariableType left_type = kVariableTypeUnknown;
  VariableType right_type = kVariableTypeUnknown;
  if (AcceptToken(1, kTokenNot)) {
    VariableType operand_type = ParseArithmeticExpression();
    if (operand_type != kVariableTypeBool) {
      puts("Type error: 'not' requires boolean.");
      token.type = kTokenEof;
    }
    ParseOperator(kTokenNot);
    return kVariableTypeBool;
  }

  left_type = ParseArithmeticExpression();

  if (kTokenEquals == token.type || kTokenNotEquals == token.type ||
      kTokenGreaterThan == token.type || kTokenGreaterOrEquals == token.type ||
      kTokenLessThan == token.type || kTokenLessOrEquals == token.type) {
    const TokenType kOperator = token.type;
    ConsumeNextToken();
    right_type = ParseArithmeticExpression();

    if (left_type != kVariableTypeInt || right_type != kVariableTypeInt) {
      puts("Type error: Comparison requires integers.");
      token.type = kTokenEof;
    }
    ParseOperator(kOperator);
    return kVariableTypeBool;
  }
  return left_type;
}

// TODO(Martin): Implement logical expressions.
// NOLINTNEXTLINE(misc-no-recursion)
static VariableType ParseLogicalExpression() {
  return ParseRelationalExpression();
}

// NOLINTNEXTLINE(misc-no-recursion)
static VariableType ParseExpression() { return ParseLogicalExpression(); }

// NOLINTNEXTLINE(misc-no-recursion)
static void ParsePrintStatement() {
  if (!ExpectToken(1, kTokenLeftParenthesis)) {
    return;
  }

  ParseExpression();

  if (!ExpectToken(1, kTokenRightParenthesis)) {
    return;
  }

  EmitByte(kOpPrint);
}

// clang-format off
#pragma static-locals(push, off)
// clang-format on
// NOLINTNEXTLINE(misc-no-recursion)
static void ParseIfStatement() {
  size_t condition_patch_slot = 0;
  size_t exit_patch_slot = 0;

  if (!ExpectToken(1, kTokenLeftParenthesis)) {
    return;
  }

  ParseExpression();

  if (!ExpectToken(1, kTokenRightParenthesis)) {
    return;
  }

  EmitByte(kOpJumpIfFalse);

  condition_patch_slot = instruction_address;

  EmitByte(0);

  while (kTokenEndif != token.type && kTokenElse != token.type &&
         kTokenEof != token.type) {
    ParseStatement();
  }

  if (kTokenElse != token.type) {
    instructions[condition_patch_slot] = instruction_address;

    ExpectToken(1, kTokenEndif);

    return;
  }

  EmitByte(kOpJump);

  exit_patch_slot = instruction_address;

  EmitByte(0);

  instructions[condition_patch_slot] = instruction_address;

  ConsumeNextToken();

  while (kTokenEndif != token.type && kTokenEof != token.type) {
    ParseStatement();
  }

  instructions[exit_patch_slot] = instruction_address;

  ExpectToken(1, kTokenEndif);
}
// clang-format off
#pragma static-locals(pop)
// clang-format on

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

  EmitByte(kOpStoreGlobal);
  EmitByte(symbol_index);
  EmitByte(type);
}

static VariableType ParseArrayLiteral() {
  size_t element_count = 0;
  int value = 0;
  size_t index = 0;
  if (!ExpectToken(1, kTokenLeftBracket)) {
    return kVariableTypeUnknown;
  }

  while (token.type != kTokenRightBracket && token.type != kTokenEof) {
    if (token.type != kTokenNumber) {
      puts("Error: Only integers are supported in arrays.");
      token.type = kTokenEof;
      return kVariableTypeUnknown;
    }

    value = token.value.number;
    ConsumeNextToken();

    index = AddNumberConstant(value, kConstantTypeNumber);
    EmitByte(kOpConstant);
    EmitByte(index);
    ++element_count;

    if (token.type == kTokenComma) {
      ConsumeNextToken();
    } else if (token.type != kTokenRightBracket) {
      puts("Error: Expected ',' or ']'");
      token.type = kTokenEof;
      return kVariableTypeUnknown;
    }
  }

  if (!ExpectToken(1, kTokenRightBracket)) {
    puts("Error: Missing closing bracket for array.");
    token.type = kTokenEof;
    return kVariableTypeUnknown;
  }

  EmitByte(kOpMakeArray);
  EmitByte(element_count);

  return kVariableTypeArray;
}

static void SetVariable(const char* const identifier_name) {
  size_t index = 0;

  index = FindLocalSymbol(identifier_name);

  if (((size_t)-1 != index) && (int)is_function_scope) {
    EmitByte(kOpStoreLocal);
    EmitByte((unsigned char)index);

    // TODO(Martin): Implement type checker.
    ++instruction_address;

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
  ++instruction_address;
}

// NOLINTNEXTLINE(misc-no-recursion)
static void ParseVariableDeclaration(const char* const identifier_name) {
  // bool is_local = false;
  VariableType variable_type = 0;
  VariableType expr_type = kVariableTypeUnknown;

  variable_type = TokenTypeToVariableType(token.type);

  if (!ExpectToken(5, kTokenInt, kTokenStr, kTokenBool, kTokenFloat,
                   kTokenArray)) {
    return;
  }

  if (!ExpectToken(1, kTokenAssign)) {
    return;
  }

  if (AcceptToken(1, kTokenFunc)) {
    ParseFunctionDefinition(identifier_name, variable_type);

    return;
  }
  if (variable_type == kVariableTypeArray) {
    expr_type = ParseArrayLiteral();
  } else {
    expr_type = ParseExpression();
  }

  if (expr_type != variable_type) {
    printf("Type error: Cannot assign %s to variable of type %s.\n",
           VariableTypeToString(expr_type),
           VariableTypeToString(variable_type));
    token.type = kTokenEof;
    return;
  }

  DefineVariable(identifier_name, variable_type, false);
}

static void ParseVariableAssignment(const char* const identifier_name) {
  VariableType expected_type = kVariableTypeUnknown;
  VariableType expr_type = kVariableTypeUnknown;
  size_t index = (size_t)-1;
  bool is_local = false;

  // Check for local variable
  index = FindLocalSymbol(identifier_name);
  if (index != (size_t)-1) {
    expected_type = kVariableTypeInt;
    // NOLINTNEXTLINE(clang-analyzer-deadcode.DeadStores)
    is_local = true;
  } else {
    index = FindGlobalSymbol(identifier_name);
    if (index == (size_t)-1) {
      printf("Error: Undefined variable '%s'.\n", identifier_name);
      token.type = kTokenEof;
      return;
    }
    expected_type = symbol_table[index].type;
  }
  expr_type = ParseExpression();

  if (expr_type != expected_type) {
    printf("Type error: Cannot assign %s to variable '%s' of type %s.\n",
           VariableTypeToString(expr_type), identifier_name,
           VariableTypeToString(expected_type));
    token.type = kTokenEof;
    return;
  }

  SetVariable(identifier_name);
}

// NOLINTNEXTLINE(misc-no-recursion,readability-function-cognitive-complexity)
static void ParseFunctionDefinition(const char* const identifier_name,
                                    const VariableType return_type) {
  size_t symbol_address = 0;
  size_t arity = 0;
  size_t arity_index = 0;
  size_t jump_address = 0;
  size_t body_start_address = 0;

  if (!ExpectToken(1, kTokenLeftParenthesis)) {
    return;
  }

  is_function_scope = true;

  // Parse parameters
  while (token.type != kTokenRightParenthesis && token.type != kTokenEof) {
    char parameter_identifier_name[kIdentifierNameLength];
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

    // NOLINTNEXTLINE(clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling)
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

    if (!(AcceptToken(1, kTokenComma) && ExpectToken(1, kTokenIdentifier))) {
      return;
    }
  }

  // In case the parameter list was empty, we need to accept the right
  // parenthesis here.
  AcceptToken(1, kTokenRightParenthesis);

  jump_address = instruction_address;
  EmitByte(kOpJump);
  EmitByte(0);

  body_start_address = instruction_address;
  EmitByte(kOpPushCallFrame);
  EmitByte(arity);
  EmitByte(return_type);

  // Emit kOpStoreLocal in reverse order for all arguments.
  for (arity_index = arity; arity_index > 0; --arity_index) {
    EmitByte(kOpStoreLocal);
    EmitByte((unsigned char)arity_index - 1);

    // TODO(Martin): Implement type checker.
    ++instruction_address;
  }

  while (kTokenEndfunc != token.type && kTokenEof != token.type) {
    ParseStatement();
  }

  if (!ExpectToken(1, kTokenEndfunc)) {
    return;
  }

  EmitByte(kOpReturn);

  //
  instructions[jump_address + 1] = instruction_address;

  symbol_address = AddSymbol(identifier_name, return_type);

  EmitByte(kOpDefineFunction);
  EmitByte(symbol_address);
  EmitByte(body_start_address);
  EmitByte(arity);
  EmitByte(return_type);
}

static void ParseReturnStatement() {
  ParseExpression();
  EmitByte(kOpReturn);
}

// NOLINTNEXTLINE(misc-no-recursion)
static void ParseIdentifierStatement(const char* const identifier_name) {
  VariableType value_type = kVariableTypeUnknown;
  if (AcceptToken(1, kTokenColon)) {
    ParseVariableDeclaration(identifier_name);

    return;
  }

  if (AcceptToken(1, kTokenLeftBracket)) {
    VariableType index_type = ParseExpression();
    size_t index = 0;

    if (index_type != kVariableTypeInt) {
      puts("Type error: Array index must be integer.");
      token.type = kTokenEof;
      return;
    }

    if (!ExpectToken(1, kTokenRightBracket)) {
      return;
    }

    if (!ExpectToken(1, kTokenAssign)) {
      return;
    }

    value_type = ParseExpression();

    if (value_type != kVariableTypeInt) {
      puts("Type error: Only integer values can be stored in arrays.");
      token.type = kTokenEof;
      return;
    }

    index = FindLocalSymbol(identifier_name);
    if (index != (size_t)-1) {
      EmitByte(kOpLoadLocal);
      EmitByte((unsigned char)index);
    } else {
      index = FindGlobalSymbol(identifier_name);
      if (index == (size_t)-1) {
        printf("Error: Undefined variable '%s'.\n", identifier_name);
        token.type = kTokenEof;
        return;
      }

      EmitByte(kOpLoadGlobal);
      EmitByte((unsigned char)index);
    }

    EmitByte(kOpStoreElement);
    return;
  }

  if (AcceptToken(1, kTokenAssign)) {
    ParseVariableAssignment(identifier_name);

    return;
  }

  puts("Error: Unexpected token after identifier.");
}

// NOLINTNEXTLINE(misc-no-recursion)
static void ParseForStatement() {
  size_t jump_if_false_patch = 0;
  size_t loop_condition_address = 0;
  size_t increment_start_address = 0;
  size_t jump_after_increment_patch = 0;
  size_t after_loop_address = 0;
  char identifier_name[kIdentifierNameLength];

  if (!ExpectToken(1, kTokenLeftParenthesis)) {
    return;
  }

  // Parse variable declaration
  // Initializer example: i: int = 0
  ExtractIdentifierName(identifier_name);
  if (!ExpectToken(1, kTokenIdentifier)) {
    return;
  }

  if (!ExpectToken(1, kTokenColon)) {
    return;
  }

  ParseVariableDeclaration(identifier_name);

  if (!ExpectToken(1, kTokenSemicolon)) {
    puts("That's yap!: You're missing a semicolon after the initializer.");
    return;
  }

  // Parse condition address
  loop_condition_address = instruction_address;
  ParseExpression();  // Condition example: i < 3

  EmitByte(kOpJumpIfFalse);
  jump_if_false_patch = instruction_address;
  EmitByte(0);  // patched after loop body

  if (!ExpectToken(1, kTokenSemicolon)) {
    puts("That's yap!: You're missing a semicolon after the condition.");
    return;
  }

  // Parse increment & store increment skip address
  // Increment example: i = i + 1
  EmitByte(kOpJump);
  jump_after_increment_patch = instruction_address;
  EmitByte(0);  // patched to skip increment on false condition

  increment_start_address = instruction_address;

  ExtractIdentifierName(identifier_name);
  if (!ExpectToken(1, kTokenIdentifier)) {
    return;
  }

  if (!ExpectToken(1, kTokenAssign)) {
    return;
  }

  ParseVariableAssignment(identifier_name);

  if (!ExpectToken(1, kTokenRightParenthesis)) {
    return;
  }

  // Jump from end of increment back to condition
  EmitByte(kOpJump);
  EmitByte(loop_condition_address);

  // Patch earlier jump to skip increment on first entry
  instructions[jump_after_increment_patch] = instruction_address;

  // Parse loop body
  while (token.type != kTokenEndfor && token.type != kTokenEof) {
    ParseStatement();  // Body example: print(i)
  }

  // Jump from body to increment
  EmitByte(kOpJump);
  EmitByte(increment_start_address);

  // === PATCHING ===
  after_loop_address = instruction_address;
  instructions[jump_if_false_patch] = after_loop_address;

  ExpectToken(1, kTokenEndfor);

  EmitHalt();
}

// NOLINTNEXTLINE(misc-no-recursion)
static void ParseWhileStatement() {
  size_t loop_start_index = 0;
  size_t pending_loop_exit_slot = 0;

  // Mark where the loop starts
  loop_start_index = instruction_address;

  // Parse condition
  // condition example: i < 3
  if (!ExpectToken(1, kTokenLeftParenthesis)) {
    return;
  }

  ParseExpression();

  if (!ExpectToken(1, kTokenRightParenthesis)) {
    return;
  }

  // Emit conditional jump to exit if false
  EmitByte(kOpJumpIfFalse);
  pending_loop_exit_slot = instruction_address;
  EmitByte(0);  // placeholder to be patched

  // Parse loop body
  while (token.type != kTokenEndwhile && token.type != kTokenEof) {
    ParseStatement();
  }

  // Jump back to start of loop
  EmitByte(kOpJump);
  EmitByte(loop_start_index);

  // Waiting for instruction jump to jump to loop exit
  instructions[pending_loop_exit_slot] = instruction_address;

  ExpectToken(1, kTokenEndwhile);
}

// NOLINTNEXTLINE(misc-no-recursion)
static void ParseStatement() {
  char identifier_name[kIdentifierNameLength];

  if (AcceptToken(1, kTokenPrint)) {
    ParsePrintStatement();

    return;
  }

  if (AcceptToken(1, kTokenIf)) {
    ParseIfStatement();

    return;
  }

  if (AcceptToken(1, kTokenRet)) {
    ParseReturnStatement();

    return;
  }

  if (AcceptToken(1, kTokenFor)) {
    ParseForStatement();

    return;
  }

  if (AcceptToken(1, kTokenWhile)) {
    ParseWhileStatement();

    return;
  }

  ExtractIdentifierName(identifier_name);

  if (AcceptToken(1, kTokenIdentifier)) {
    if (token.type == kTokenLeftBracket) {
      VariableType result_type = ParseIdentifier(identifier_name);
      (void)result_type;
      return;
    }

    if (kTokenColon == token.type || kTokenAssign == token.type) {
      ParseIdentifierStatement(identifier_name);

      return;
    }

    ParseExpression();
  }

  printf("Error: Unregistered statement '%s'.\n", identifier_name);

  token.type = kTokenEof;
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
