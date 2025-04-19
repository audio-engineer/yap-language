#include "parser_test.h"

#include <string.h>
#include <unity.h>
#include <vm.h>

#include "global.h"

size_t NextConstant() { return ++constants_index; }

static void TestBinaryOperator(const char* const source_code,
                               const Opcode operator_opcode) {
  FillProgramBufferAndParse(source_code);

  constants_index = 0;

  const unsigned char kExpectedOpcodes[kInstructionsSize] = {
      kOpConstant,     constants_index, kOpConstant, NextConstant(),
      operator_opcode, kOpPrint,        kOpHalt};

  TEST_ASSERT_EQUAL_CHAR_ARRAY(kExpectedOpcodes, instructions,
                               kInstructionsSize);
}

static void TestError(const char* const source_code) {
  FillProgramBufferAndParse(source_code);

  constants_index = 0;

  constexpr unsigned char kExpectedOpcodes[kInstructionsSize] = {kOpHalt};

  TEST_ASSERT_EQUAL_CHAR_ARRAY(kExpectedOpcodes, instructions,
                               kInstructionsSize);
}

void TestRecursiveArithmetic() {
  FillProgramBufferAndParse("print(6+3*5-1/1)");

  constants_index = 0;

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

  constants_index = 0;

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

  constants_index = 0;

  const unsigned char kExpectedOpcodes[kInstructionsSize] = {
      kOpConstant, constants_index, kOpPrint, kOpHalt};

  TEST_ASSERT_EQUAL_CHAR_ARRAY(kExpectedOpcodes, instructions,
                               kInstructionsSize);
}

void TestFalseBoolean() {
  FillProgramBufferAndParse("print(false)");

  constants_index = 0;

  const unsigned char kExpectedOpcodes[kInstructionsSize] = {
      kOpConstant, constants_index, kOpPrint, kOpHalt};

  TEST_ASSERT_EQUAL_CHAR_ARRAY(kExpectedOpcodes, instructions,
                               kInstructionsSize);
}

void TestStringParse() {
  FillProgramBufferAndParse("print(\"something\")");

  constants_index = 0;

  const unsigned char kExpectedOpcodes[kInstructionsSize] = {
      kOpConstant, constants_index, kOpPrint, kOpHalt};

  TEST_ASSERT_EQUAL_CHAR_ARRAY(kExpectedOpcodes, instructions,
                               kInstructionsSize);
}

void TestDeclareIntAndPrint() {
  FillProgramBufferAndParse("x: int = 5\nprint(x)");

  constants_index = 0;

  const unsigned char kExpectedOpcodes[kInstructionsSize] = {
      kOpConstant,      constants_index, kOpStoreGlobal,  constants_index,
      kVariableTypeInt, kOpLoadGlobal,   constants_index, kVariableTypeInt,
      kOpPrint,         kOpHalt};

  TEST_ASSERT_EQUAL_CHAR_ARRAY(kExpectedOpcodes, instructions,
                               kInstructionsSize);
}

void TestDeclareBool() {
  FillProgramBufferAndParse("x: bool = true");

  constants_index = 0;

  const unsigned char kExpectedOpcodes[kInstructionsSize] = {
      kOpConstant,     constants_index,   kOpStoreGlobal,
      constants_index, kVariableTypeBool, kOpHalt};

  TEST_ASSERT_EQUAL_CHAR_ARRAY(kExpectedOpcodes, instructions,
                               kInstructionsSize);
}

void TestDeclareStr() {
  FillProgramBufferAndParse("x: str = \"hello\"");

  constants_index = 0;

  const unsigned char kExpectedOpcodes[kInstructionsSize] = {
      kOpConstant,     constants_index,  kOpStoreGlobal,
      constants_index, kVariableTypeStr, kOpHalt};

  TEST_ASSERT_EQUAL_CHAR_ARRAY(kExpectedOpcodes, instructions,
                               kInstructionsSize);
}

void TestDeclareFloat() {
  FillProgramBufferAndParse("x: float = 2.2");

  constants_index = 0;

  const unsigned char kExpectedOpcodes[kInstructionsSize] = {
      kOpConstant,     constants_index,    kOpStoreGlobal,
      constants_index, kVariableTypeFloat, kOpHalt};

  TEST_ASSERT_EQUAL_CHAR_ARRAY(kExpectedOpcodes, instructions,
                               kInstructionsSize);
}

void TestDeclareIntAssignAndPrint() {
  ResetInterpreterState();

  FillProgramBufferAndParse("x: int = 5\nx = 6\nprint(x)");

  constants_index = 0;

  const unsigned char kExpectedOpcodes[kInstructionsSize] = {
      kOpConstant,       constants_index,  kOpStoreGlobal, constants_index,
      kVariableTypeInt,  kOpConstant,      NextConstant(), kOpStoreGlobal,
      --constants_index, kVariableTypeInt, kOpLoadGlobal,  constants_index,
      kVariableTypeInt,  kOpPrint,         kOpHalt};

  TEST_ASSERT_EQUAL_CHAR_ARRAY(kExpectedOpcodes, instructions,
                               kInstructionsSize);
}

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
      0,
      kVariableTypeInt,
      kOpStoreLocal,
      1,
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
      kOpStoreLocal,
      1,
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
      kOpLoadGlobal,
      0,
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

void TestUnregisteredStatement() { TestError("prant(3+5)"); }

void TestMissingLeftParen() { TestError("print3+5)"); }
