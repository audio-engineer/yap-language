#include "vm_test.h"

#ifdef __linux__
#include <stddef.h>
#elif __APPLE__
#include <sys/_types/_size_t.h>
#endif
#include <unity.h>
#include <vm.h>

// NOLINTNEXTLINE(bugprone-suspicious-include,-warnings-as-errors)
#include <vm.c>

#include "global.h"

void TestVMArithmetic(const int expected, const char* code) {
  FillProgramBufferAndParse(code);
  RunVm();

  TEST_ASSERT_EQUAL(expected, stack[stack_index].as.number);

  ResetInterpreterState();
}

void TestConditionalResult(const int expected, const char* const code) {
  FillProgramBufferAndParse(code);
  RunVm();

  TEST_ASSERT_EQUAL(expected, global_variables[0].as.number);

  ResetInterpreterState();
}

void TestPlusOperator() { TestVMArithmetic(4, "x:int=2+2"); }

void TestMinusOperator() { TestVMArithmetic(1, "x:int=3-2"); }

// NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers,-warnings-as-errors)
void TestMultiplyOperator() { TestVMArithmetic(6, "x:int=2*3"); }

void TestDivideOperator() { TestVMArithmetic(2, "x:int=4/2"); }

void TestModuloOperator() { TestVMArithmetic(1, "x:int=5%2"); }

void TestEqualOperator() {
  TestVMArithmetic(1, "x:bool = 2 == 2");
  TestVMArithmetic(0, "x:bool = 2 == 3");
}

void TestNotEqualOperator() {
  TestVMArithmetic(0, "x:bool = 2 != 2");
  TestVMArithmetic(1, "x:bool = 2 != 3");
}

void TestGreaterThanOperator() {
  TestVMArithmetic(0, "x:bool = 2 > 3");
  TestVMArithmetic(1, "x:bool = 3 > 2");
}

void TestGreaterOrEqualsOperator() {
  TestVMArithmetic(0, "x:bool = 2 >= 3");
  TestVMArithmetic(1, "x:bool = 3 >= 2");
  TestVMArithmetic(1, "x:bool = 3 >= 3");
}

void TestLessThanOperator() {
  TestVMArithmetic(1, "x:bool = 2 < 3");
  TestVMArithmetic(0, "x:bool = 3 < 2");
}

void TestLessOrEqualsOperator() {
  TestVMArithmetic(1, "x:bool = 2 <= 3");
  TestVMArithmetic(0, "x:bool = 3 <= 2");
  TestVMArithmetic(1, "x:bool = 3 <= 3");
}

void TestOrOperator() {
  TestVMArithmetic(1, "x:bool = true || false");
  TestVMArithmetic(1, "x:bool = false || true");
  TestVMArithmetic(0, "x:bool = false || false");
}

void TestAndOperator() {
  TestVMArithmetic(1, "x:bool = true && true");
  TestVMArithmetic(0, "x:bool = true && false");
  TestVMArithmetic(0, "x:bool = false && true");
  TestVMArithmetic(0, "x:bool = false && false");
}

void TestIfTrueExecutesBlock() {
  // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers,-warnings-as-errors)
  TestConditionalResult(42,
                        "x: int = 0"
                        "if(true)"
                        "  x = 42"
                        "else"
                        "  x = 99"
                        "endif"
                        "print(x)");
}

void TestIfFalseSkipsBlock() {
  TestConditionalResult(0,
                        "x: int = 0\n"
                        "if(false)\n"
                        "  x = 123\n"
                        "endif\n"
                        "print(x)");
}

void TestIfGreaterThanComparison() {
  TestConditionalResult(1,
                        "x: int = 0\n"
                        "if(5 > 2)\n"
                        "  x = 1\n"
                        "endif\n"
                        "print(x)");
}

void TestIfLessThanComparison() {
  TestConditionalResult(0,
                        "x: int = 0\n"
                        "if(1 < 0)\n"
                        "  x = 1\n"
                        "endif\n"
                        "print(x)");
}

void TestElse() {
  // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers,-warnings-as-errors)
  TestConditionalResult(99,
                        "x: int = 0\n"
                        "if(false)\n"
                        "  x = 42\n"
                        "else\n"
                        "  x = 99\n"
                        "endif\n"
                        "print(x)");
}

void TestNestedConditionals() {
  // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers,-warnings-as-errors)
  TestConditionalResult(222,
                        "x: int = 0\n"
                        "if(true)\n"
                        "  if(false)\n"
                        "    x = 111\n"
                        "  else\n"
                        "    x = 222\n"
                        "  endif\n"
                        "else\n"
                        "  x = 999\n"
                        "endif\n"
                        "print(x)");
}

// Acceptance testing

void Example1() {
  FillProgramBufferAndParse(
      "add: int = func(x: int, y: int) \n "
      "ret x + y \n "
      "endfunc \n"
      "x:int = add(5, 6)\n "
      "run");

  RunVm();

  TEST_ASSERT_EQUAL(11, stack[stack_index].as.number);

  ResetInterpreterState();
}

void Example2() {
  FillProgramBufferAndParse(
      "a: int = 2 \n "
      "a = a + 1 \n "
      "run");

  RunVm();

  TEST_ASSERT_EQUAL(3, stack[stack_index].as.number);

  ResetInterpreterState();
}

void Example3() {
  FillProgramBufferAndParse(
      "foo: int = func(num: int)\n"
      "ret (num + 2) * 3 \n"
      "endfunc\n"
      "b: int = foo(8) \n"
      "run");

  RunVm();

  TEST_ASSERT_EQUAL(30, stack[stack_index].as.number);

  ResetInterpreterState();
}

void Example4() {
  FillProgramBufferAndParse(
      "if (true)\n"
      "  if (false)\n"
      "    x:str = \"foo\"\n"
      "  else \n"
      "    x:str = \"bar\"\n"
      "  endif\n"
      "endif\n"
      "run");

  RunVm();

  TEST_ASSERT_EQUAL_CHAR_ARRAY("bar", stack[stack_index].as.string, 4);

  ResetInterpreterState();
}

void Example5() {
  FillProgramBufferAndParse(
      "if (false)\n"
      "  if (true)\n"
      "    x:str = \"foo\"\n"
      "  endif \n"
      "else \n"
      "  x:str = bar\n"
      "endif\n"
      "run");

  RunVm();

  TEST_ASSERT_EQUAL_CHAR_ARRAY("bar", stack[stack_index].as.string, 4);

  ResetInterpreterState();
}

void Example6() {
  FillProgramBufferAndParse(
      "a: int = 1\n"
      "b: int = 2\n"
      "c: int = 3\n"
      "b = c\n"
      "a = b\n"
      "run");

  RunVm();

  TEST_ASSERT_EQUAL(3, stack[stack_index].as.number);

  ResetInterpreterState();
}

void Example7() {
  FillProgramBufferAndParse(
      "findGCD: int = func(a: int, b: int)\n"
      "  if(a == 0)\n"
      "    return b\n"
      "  endif\n"
      "  return findGCD(b % a, a)\n"
      "endfunc\n"
      "x: int = findGCD(35, 15)\n"
      "run");

  RunVm();

  TEST_ASSERT_EQUAL(5, stack[stack_index].as.number);

  ResetInterpreterState();
}

void Example8() {
  FillProgramBufferAndParse(
      "isPrime: bool = func(n: int)\n"
      "  if(n <= 1)\n"
      "    return false\n"
      "  endif\n"
      "  for(i: int = 2; i < n; i = i + 1)\n"
      "    if(n % i == 0)\n"
      "      return false\n"
      "    endif\n"
      "  endfor\n"
      "  return true\n"
      "endfunc\n"
      "x: bool = isPrime(90)\n"
      "y: bool = isPrime(97)\n"
      "run");

  RunVm();

  TEST_ASSERT_EQUAL(0, stack[stack_index - 1].as.number);
  TEST_ASSERT_EQUAL(1, stack[stack_index].as.number);

  ResetInterpreterState();
}

void Example9() {
  FillProgramBufferAndParse(
      "fib: int = func(n: int)\n"
      "  if(n <= 1)\n"
      "    return n\n"
      "  endif\n"
      "  return fib(n-1) + fib(n-2)\n"
      "endfunc\n"
      "x: int = fib(10)\n"
      "run");

  RunVm();

  TEST_ASSERT_EQUAL(55, stack[stack_index].as.number);

  ResetInterpreterState();
}

void Example10() {
  FillProgramBufferAndParse(
      "factorial: int = func(n: int)\n"
      "  if(n <= 1)\n"
      "    return 1\n"
      "  endif\n"
      "  return n * factorial(n - 1)\n"
      "endfunc\n"
      "x: int = factorial(5)\n"
      "run");

  RunVm();

  TEST_ASSERT_EQUAL(120, stack[stack_index].as.number);

  ResetInterpreterState();
}

// Loops testing

void TestForLoopExecutesThreeTimes(void) {
  FillProgramBufferAndParse(
      "for(i: int = 0; i < 3; i = i + 1)\n"
      "  print(i)\n"
      "endfor");

  int print_count = 0;
  int jump_count = 0;
  bool saw_jump_if_false = false;

  for (size_t i = 0; i < kInstructionsSize; ++i) {
    if (instructions[i] == kOpPrint) {
      print_count++;
    }
    if (instructions[i] == kOpJumpIfFalse) {
      saw_jump_if_false = true;
    }
    if (instructions[i] == kOpJump) {
      jump_count++;
    }
  }

  TEST_ASSERT_TRUE_MESSAGE(saw_jump_if_false,
                           "Missing kOpJumpIfFalse for for-loop condition");
  TEST_ASSERT_TRUE_MESSAGE(jump_count >= 2,
                           "Expected at least two kOpJump instructions (one to "
                           "body, one to increment)");
  TEST_ASSERT_EQUAL_INT_MESSAGE(1, print_count,
                                "Expected 1 print instruction in bytecode");

  RunVm();
}

void TestWhileLoopExecutesThreeTimes() {
  FillProgramBufferAndParse(
      "i: int = 0\n"
      "while(i < 3)\n"
      "  print(i)\n"
      "  i = i + 1\n"
      "endwhile");

  int print_count = 0;
  bool saw_jump_if_false = false;
  bool saw_jump = false;

  for (size_t i = 0; i < kInstructionsSize; ++i) {
    if (instructions[i] == kOpPrint) {
      print_count++;
    }
    if (instructions[i] == kOpJumpIfFalse) {
      saw_jump_if_false = true;
    }
    if (instructions[i] == kOpJump) {
      saw_jump = true;
    }
  }

  TEST_ASSERT_TRUE_MESSAGE(saw_jump_if_false,
                           "Missing kOpJumpIfFalse for while");
  TEST_ASSERT_TRUE_MESSAGE(saw_jump, "Missing kOpJump to repeat loop");
  TEST_ASSERT_EQUAL_INT_MESSAGE(1, print_count,
                                "Expected 1 print instruction in bytecode");

  RunVm();
}

void TestNestedWhileLoops() {
  // Expected output should be:
  // 0
  // 1
  // 0
  // 1

  FillProgramBufferAndParse(
      "i: int = 0\n"
      "j: int = 0\n"
      "while(i < 2)\n"
      "  j = 0\n"
      "  while(j < 2)\n"
      "    print(j)\n"
      "    j = j + 1\n"
      "  endwhile\n"
      "  i = i + 1\n"
      "endwhile");

  RunVm();
}
