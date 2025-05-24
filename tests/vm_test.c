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

void TestPlus() { TestVMArithmetic(4, "x:int=2+2"); }

void TestMinusVM() { TestVMArithmetic(1, "x:int=3-2"); }

// NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers,-warnings-as-errors)
void TestMultiply() { TestVMArithmetic(6, "x:int=2*3"); }

void TestDivide() { TestVMArithmetic(2, "x:int=4/2"); }

void TestModulo() { TestVMArithmetic(1, "x:int=5%2"); }

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
