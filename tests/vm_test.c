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

void TestPlus() { TestVMArithmetic(4, "x:int=2+2"); }

void TestMinusVM() { TestVMArithmetic(1, "x:int=3-2"); }

// NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers,-warnings-as-errors)
void TestMultiply() { TestVMArithmetic(6, "x:int=2*3"); }

void TestDivide() { TestVMArithmetic(2, "x:int=4/2"); }

void TestModulo() { TestVMArithmetic(1, "x:int=5%2"); }

void TestIfTrueExecutesBlock() {
  FillProgramBufferAndParse("if (1) print(\"yes\") endif");

  bool saw_jump_if_false = false;
  bool saw_print = false;
  bool saw_halt = false;

  for (size_t i = 0; i < kInstructionsSize; ++i) {
    if (instructions[i] == kOpJumpIfFalse) {
      saw_jump_if_false = true;
    }

    if (instructions[i] == kOpPrint) {
      saw_print = true;
    }

    if (instructions[i] == kOpHalt) {
      saw_halt = true;
    }
  }

  TEST_ASSERT_TRUE_MESSAGE(saw_jump_if_false,
                           "Missing kOpJumpIfFalse for 'if' check");
  TEST_ASSERT_TRUE_MESSAGE(saw_print, "Missing kOpPrint inside 'if' block");
  TEST_ASSERT_TRUE_MESSAGE(saw_halt, "Missing kOpHalt at end");

  RunVm();
}

void TestIfFalseSkipsBlock() {
  FillProgramBufferAndParse("if (0) print(\"no\") endif");

  bool saw_print = false;
  bool saw_jump = true;

  for (size_t i = 0; i < kInstructionsSize; ++i) {
    if (instructions[i] == kOpPrint) {
      saw_print = true;
    }

    if (instructions[i] == kOpJump) {
      saw_jump = false;
    }
  }

  TEST_ASSERT_TRUE_MESSAGE(saw_print, "Missing kOpPrint inside 'if' block");
  TEST_ASSERT_TRUE_MESSAGE(saw_jump, "kOpJump should not exist in bytecode");

  RunVm();
}

void TestIfGreaterThanComparison() {
  FillProgramBufferAndParse("if (5 > 2) print(\"greater\") endif");

  RunVm();
}

void TestIfLessThanComparison() {
  FillProgramBufferAndParse("if (1 < 0) print(\"should not appear\") endif");

  RunVm();
}

void TestElse() {
  FillProgramBufferAndParse("if(3>2) print(\"foo\") else print(\"bar\") endif");

  RunVm();
}

// Loops testing

/*void TestForLoopExecutesThreeTimes() {
  int print_occurences = 0;

  FillProgramBufferAndParse(
      "for(i: int = 0; i < 3; i = i + 1)\n"
      "  print(i)\n"
      "endfor");

  bool saw_jump = false;
  bool saw_jump_if_false = false;

  for (size_t i = 0; i < kInstructionsSize; ++i) {
    if (instructions[i] == kOpPrint) {
      print_occurences++;
    }
    if (instructions[i] == kOpJump) {
      saw_jump = true;
    }
    if (instructions[i] == kOpJumpIfFalse) {
      saw_jump_if_false = true;
    }
  }

  TEST_ASSERT_TRUE_MESSAGE(saw_jump, "Missing kOpJump for loop iteration");
  TEST_ASSERT_TRUE_MESSAGE(saw_jump_if_false,
                           "Missing kOpJumpIfFalse for loop condition");
  TEST_ASSERT_EQUAL_INT_MESSAGE(3, print_occurences, "Loop did not print 3 times");

  RunVm();
}
*/
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

  TEST_ASSERT_TRUE_MESSAGE(saw_jump_if_false, "Missing kOpJumpIfFalse for while");
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


