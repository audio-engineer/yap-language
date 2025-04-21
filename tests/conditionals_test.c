#ifdef __linux__
#include <stddef.h>
#elif __APPLE__
#include <sys/_types/_size_t.h>
#endif
#include <unity.h>
#include <vm.h>

#include "conditionals_test.h"
#include "global.h"

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
