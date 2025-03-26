#include "parser.h"

#include <string.h>
#include <unity.h>
#include <vm.h>

#include "parser_test.h"

// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables,-warnings-as-errors)
static size_t constant_index = 0;

static size_t NextConstant() { return constant_index++; }

static void ResetTest() {
  // NOLINTNEXTLINE(clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling)
  memset(instructions, 0, kInstructionsSize * sizeof(char));

  instruction_index = 0;
}

void TestRecursiveArithmetic() {
  ParseProgram("print(6+3*5-1/1)");

  const unsigned char kExpectedOpcodes[kInstructionsSize] = {
      kOpConstant, NextConstant(), kOpConstant, NextConstant(),
      kOpConstant, NextConstant(), kOpMultiply, kOpAdd,
      kOpConstant, NextConstant(), kOpConstant, NextConstant(),
      kOpDivide,   kOpSubtract,    kOpPrint};

  TEST_ASSERT_EQUAL_CHAR_ARRAY(kExpectedOpcodes, instructions,
                               kInstructionsSize);

  ResetTest();
}

static void TestCondition(const char* source_code, const Opcode opcode) {
  ParseProgram(source_code);

  const unsigned char kExpectedOpcodes[kInstructionsSize] = {
      kOpConstant,    NextConstant(), kOpConstant,
      NextConstant(), opcode,         kOpPrint};

  TEST_ASSERT_EQUAL_CHAR_ARRAY(kExpectedOpcodes, instructions,
                               kInstructionsSize);

  ResetTest();
}

void TestAddArithmetic() { TestCondition("print(5 + 10)", kOpAdd); }

void TestSubtractArithmetic() { TestCondition("print(5 - 10)", kOpSubtract); }

void TestMultiplyArithmetic() { TestCondition("print(5 * 10)", kOpMultiply); }

void TestDivideArithmetic() { TestCondition("print(10 / 5)", kOpDivide); }

void TestLessThanCondition() { TestCondition("print(1 < 10)", kOpLessThan); }

void TestLessOrEqualCondition() {
  TestCondition("print(5 <= 10)", kOpLessOrEquals);
}

void TestGreaterThanCondition() {
  TestCondition("print(1 > 10)", kOpGreaterThan);
}

void TestGreaterOrEqualCondition() {
  TestCondition("print(5 >= 10)", kOpGreaterOrEquals);
}

void TestEqualCondition() { TestCondition("print(1 == 10)", kOpEquals); }

void TestNotEqualCondition() { TestCondition("print(1 != 10)", kOpNotEquals); }

void TestTrueBoolean() {
  ParseProgram("print(true)");

  const unsigned char kExpectedOpcodes[kInstructionsSize] = {
      kOpConstant, NextConstant(), kOpPrint};

  TEST_ASSERT_EQUAL_CHAR_ARRAY(kExpectedOpcodes, instructions,
                               kInstructionsSize);

  ResetTest();
}

void TestFalseBoolean() {
  ParseProgram("print(false)");

  const unsigned char kExpectedOpcodes[kInstructionsSize] = {
      kOpConstant, NextConstant(), kOpPrint};

  TEST_ASSERT_EQUAL_CHAR_ARRAY(kExpectedOpcodes, instructions,
                               kInstructionsSize);

  ResetTest();
}

void TestStringParse() {
  ParseProgram("print(\"something\")");

  const unsigned char kExpectedOpcodes[kInstructionsSize] = {
      kOpConstant, NextConstant(), kOpPrint};

  TEST_ASSERT_EQUAL_CHAR_ARRAY(kExpectedOpcodes, instructions,
                               kInstructionsSize);

  ResetTest();
}

static void TestError(const char* source_code) {
  ParseProgram(source_code);

  constexpr unsigned char kExpectedOpcodes[kInstructionsSize] = {};

  TEST_ASSERT_EQUAL_CHAR_ARRAY(kExpectedOpcodes, instructions,
                               kInstructionsSize);

  ResetTest();
}

void TestUnregisteredStatement() { TestError("prant(3+5)"); }

void TestNoExpression() { TestError("print(+5)"); }

void TestMissingLeftParen() { TestError("print3+5)"); }
