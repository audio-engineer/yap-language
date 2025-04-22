#include "parser_test.h"

#include <string.h>
#include <unity.h>
#include <vm.h>

#include "global.h"

static size_t NextConstant() { return ++constants_index; }

static void TestBinaryOperator(const char* const source_code,
                               const Opcode operator_opcode) {
  FillProgramBufferAndParse(source_code);

  const unsigned char kExpectedOpcodes[kInstructionsSize] = {
      kOpConstant,     constants_index, kOpConstant, NextConstant(),
      operator_opcode, kOpPrint,        kOpHalt};

  TEST_ASSERT_EQUAL_CHAR_ARRAY(kExpectedOpcodes, instructions,
                               kInstructionsSize);
}

void TestRecursiveArithmetic() {
  FillProgramBufferAndParse("print(6+3*5-1/1)");

  const unsigned char kExpectedOpcodes[kInstructionsSize] = {
      kOpConstant, constants_index, kOpConstant, NextConstant(),
      kOpConstant, NextConstant(),  kOpMultiply, kOpAdd,
      kOpConstant, NextConstant(),  kOpConstant, NextConstant(),
      kOpDivide,   kOpSubtract,     kOpPrint,    kOpHalt};

  TEST_ASSERT_EQUAL_CHAR_ARRAY(kExpectedOpcodes, instructions,
                               kInstructionsSize);
}

void TestParenthesesArithmetic() {
  FillProgramBufferAndParse("print((4 + 2) * (4 - 2) / 2)");

  const unsigned char kExpectedOpcodes[kInstructionsSize] = {
      kOpConstant,    constants_index, kOpConstant,    NextConstant(),
      kOpAdd,         kOpConstant,     NextConstant(), kOpConstant,
      NextConstant(), kOpSubtract,     kOpMultiply,    kOpConstant,
      NextConstant(), kOpDivide,       kOpPrint,       kOpHalt};

  TEST_ASSERT_EQUAL_CHAR_ARRAY(kExpectedOpcodes, instructions,
                               kInstructionsSize);
}

void TestAddArithmetic() { TestBinaryOperator("print(5 + 10)", kOpAdd); }

void TestSubtractArithmetic() {
  TestBinaryOperator("print(5 - 10)", kOpSubtract);
}

void TestMultiplyArithmetic() {
  TestBinaryOperator("print(5 * 10)", kOpMultiply);
}

void TestDivideArithmetic() { TestBinaryOperator("print(10 / 5)", kOpDivide); }

void TestLessThanCondition() {
  TestBinaryOperator("print(1 < 10)", kOpLessThan);
}

void TestLessOrEqualCondition() {
  TestBinaryOperator("print(5 <= 10)", kOpLessThanOrEqualTo);
}

void TestGreaterThanCondition() {
  TestBinaryOperator("print(1 > 10)", kOpGreaterThan);
}

void TestGreaterOrEqualCondition() {
  TestBinaryOperator("print(5 >= 10)", kOpGreaterThanOrEqualTo);
}

void TestEqualCondition() { TestBinaryOperator("print(1 == 10)", kOpEquals); }

void TestNotEqualCondition() {
  TestBinaryOperator("print(1 != 10)", kOpNotEquals);
}

void TestTrueBoolean() {
  FillProgramBufferAndParse("print(true)");

  const unsigned char kExpectedOpcodes[kInstructionsSize] = {
      kOpConstant, constants_index, kOpPrint, kOpHalt};

  TEST_ASSERT_EQUAL_CHAR_ARRAY(kExpectedOpcodes, instructions,
                               kInstructionsSize);
}

void TestFalseBoolean() {
  FillProgramBufferAndParse("print(false)");

  const unsigned char kExpectedOpcodes[kInstructionsSize] = {
      kOpConstant, constants_index, kOpPrint, kOpHalt};

  TEST_ASSERT_EQUAL_CHAR_ARRAY(kExpectedOpcodes, instructions,
                               kInstructionsSize);
}

void TestStringParse() {
  FillProgramBufferAndParse("print(\"something\")");

  const unsigned char kExpectedOpcodes[kInstructionsSize] = {
      kOpConstant, constants_index, kOpPrint, kOpHalt};

  TEST_ASSERT_EQUAL_CHAR_ARRAY(kExpectedOpcodes, instructions,
                               kInstructionsSize);
}

void TestDeclareIntAndPrint() {
  FillProgramBufferAndParse("x: int = 5\nprint(x)");

  const unsigned char kExpectedOpcodes[kInstructionsSize] = {
      kOpConstant,      constants_index, kOpStoreGlobal,  constants_index,
      kVariableTypeInt, kOpLoadGlobal,   constants_index, kVariableTypeInt,
      kOpPrint,         kOpHalt};

  TEST_ASSERT_EQUAL_CHAR_ARRAY(kExpectedOpcodes, instructions,
                               kInstructionsSize);
}

void TestDeclareBool() {
  FillProgramBufferAndParse("x: bool = true");

  const unsigned char kExpectedOpcodes[kInstructionsSize] = {
      kOpConstant,     constants_index,   kOpStoreGlobal,
      constants_index, kVariableTypeBool, kOpHalt};

  TEST_ASSERT_EQUAL_CHAR_ARRAY(kExpectedOpcodes, instructions,
                               kInstructionsSize);
}

void TestDeclareStr() {
  FillProgramBufferAndParse("x: str = \"hello\"");

  const unsigned char kExpectedOpcodes[kInstructionsSize] = {
      kOpConstant,     constants_index,  kOpStoreGlobal,
      constants_index, kVariableTypeStr, kOpHalt};

  TEST_ASSERT_EQUAL_CHAR_ARRAY(kExpectedOpcodes, instructions,
                               kInstructionsSize);
}

// TODO(Martin): Enable when floats work.
// void TestDeclareFloat() {
//   FillProgramBufferAndParse("x: float = 2.2");
//
//   const unsigned char kExpectedOpcodes[kInstructionsSize] = {
//       kOpConstant,     constants_index,    kOpStoreGlobal,
//       constants_index, kVariableTypeFloat, kOpHalt};
//
//   TEST_ASSERT_EQUAL_CHAR_ARRAY(kExpectedOpcodes, instructions,
//                                kInstructionsSize);
// }

void TestDeclareIntAssignAndPrint() {
  ResetInterpreterState();

  FillProgramBufferAndParse("x: int = 5\nx = 6\nprint(x)");

  const unsigned char kExpectedOpcodes[kInstructionsSize] = {
      kOpConstant,       constants_index,  kOpStoreGlobal, constants_index,
      kVariableTypeInt,  kOpConstant,      NextConstant(), kOpStoreGlobal,
      --constants_index, kVariableTypeInt, kOpLoadGlobal,  constants_index,
      kVariableTypeInt,  kOpPrint,         kOpHalt};

  TEST_ASSERT_EQUAL_CHAR_ARRAY(kExpectedOpcodes, instructions,
                               kInstructionsSize);
}

// TODO(Martin): Enable when functions work without parameters.
// void TestDeclareFunctionWithNoParameters() {
//   FillProgramBufferAndParse("a: str = func()\nret \"hello\"\nendfunc");
//
//   constexpr size_t kJumpAddress = 7;
//   constexpr size_t kArity = 0;
//   constexpr size_t kBodyStart = 2;
//
//   constexpr unsigned char kExpectedOpcodes[kInstructionsSize] = {
//       kOpJump,
//       kJumpAddress,
//       kOpPushCallFrame,
//       kArity,
//       kVariableTypeStr,
//       kOpReturn,
//       kOpReturn,
//       kOpDefineFunction,
//       0,
//       kBodyStart,
//       kArity,
//       kVariableTypeStr,
//       kOpHalt};
//
//   TEST_ASSERT_EQUAL_CHAR_ARRAY(kExpectedOpcodes, instructions,
//                                kInstructionsSize);
// }

void TestDeclareFunctionOneParameter() {
  FillProgramBufferAndParse("something: int = func(x: int)\nret x\nendfunc");

  constexpr size_t kJumpAddress = 13;
  constexpr size_t kArity = 1;
  constexpr size_t kBodyStart = 2;

  constexpr unsigned char kExpectedOpcodes[kInstructionsSize] = {
      kOpJump,
      kJumpAddress,
      kOpPushCallFrame,
      kArity,
      kVariableTypeInt,
      kOpStoreLocal,
      0,
      kVariableTypeInt,
      kOpLoadLocal,
      0,
      kVariableTypeInt,
      kOpReturn,
      kOpReturn,
      kOpDefineFunction,
      0,
      kBodyStart,
      kArity,
      kVariableTypeInt,
      kOpHalt};

  TEST_ASSERT_EQUAL_CHAR_ARRAY(kExpectedOpcodes, instructions,
                               kInstructionsSize);
}

void TestDeclareFunctionTwoParameters() {
  FillProgramBufferAndParse(
      "add: int = func(x: int, y: int)\nret x + y\nendfunc");

  constexpr size_t kJumpAddress = 20;
  constexpr size_t kArity = 2;
  constexpr size_t kBodyStart = 2;

  constexpr unsigned char kExpectedOpcodes[kInstructionsSize] = {
      kOpJump,
      kJumpAddress,
      kOpPushCallFrame,
      kArity,
      kVariableTypeInt,
      kOpStoreLocal,
      1,
      kVariableTypeInt,
      kOpStoreLocal,
      0,
      kVariableTypeInt,
      kOpLoadLocal,
      0,
      kVariableTypeInt,
      kOpLoadLocal,
      1,
      kVariableTypeInt,
      kOpAdd,
      kOpReturn,
      kOpReturn,
      kOpDefineFunction,
      0,
      kBodyStart,
      kArity,
      kVariableTypeInt,
      kOpHalt};

  TEST_ASSERT_EQUAL_CHAR_ARRAY(kExpectedOpcodes, instructions,
                               kInstructionsSize);
}

void TestDeclareAndCallFunction() {
  FillProgramBufferAndParse(
      "add: int = func(x: int, y: int)\nret x + y\nendfunc\nprint(add(5, 6))");

  constexpr size_t kJumpAddress = 20;
  constexpr size_t kArity = 2;
  constexpr size_t kFunctionIndex = 0;
  constexpr size_t kBodyStart = 2;

  constexpr unsigned char kExpectedOpcodes[kInstructionsSize] = {
      kOpJump,
      kJumpAddress,
      kOpPushCallFrame,
      kArity,
      kVariableTypeInt,
      kOpStoreLocal,
      1,
      kVariableTypeInt,
      kOpStoreLocal,
      0,
      kVariableTypeInt,
      kOpLoadLocal,
      0,
      kVariableTypeInt,
      kOpLoadLocal,
      1,
      kVariableTypeInt,
      kOpAdd,
      kOpReturn,
      kOpReturn,
      kOpDefineFunction,
      kFunctionIndex,
      kBodyStart,
      kArity,
      kVariableTypeInt,
      kOpLoadGlobal,
      kFunctionIndex,
      kVariableTypeInt,
      kOpConstant,
      0,
      kOpConstant,
      1,
      kOpCallFunction,
      kArity,
      kOpPrint,
      kOpHalt};

  TEST_ASSERT_EQUAL_CHAR_ARRAY(kExpectedOpcodes, instructions,
                               kInstructionsSize);
}

void TestUnregisteredStatement() {
  FillProgramBufferAndParse("prant(3+5)");

  const unsigned char kExpectedOpcodes[kInstructionsSize] = {
      kOpConstant,    constants_index, kOpConstant,
      NextConstant(), kOpAdd,          kOpHalt};

  TEST_ASSERT_EQUAL_CHAR_ARRAY(kExpectedOpcodes, instructions,
                               kInstructionsSize);
}

void TestMissingLeftParen() {
  FillProgramBufferAndParse("print3+5)");

  constexpr unsigned char kExpectedOpcodes[kInstructionsSize] = {kOpHalt};

  TEST_ASSERT_EQUAL_CHAR_ARRAY(kExpectedOpcodes, instructions,
                               kInstructionsSize);
}
