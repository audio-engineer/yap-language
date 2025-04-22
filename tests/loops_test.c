#include <lexer_test.h>
#include <unity.h>

#include "loops_test.h"
#include "parser.h"
#include "vm.h"

// Yoinked from Conditions tests
static void Run(const char* code) {
  ResetInterpreterState();

  SetTest(code);          // Sets program_buffer and resets index
  ParseProgram();         // Parses entire program
  EmitByte(kOpHalt);      // Appends HALT for VM safety
  RunVm();            // Removes need for RunVm call in every test.
}

void TestIfFalseConditionRuns() {
  Run("while(0) print(123) endwhile");

  bool saw_jump_if_false = false;
  bool saw_jump = false;
  bool saw_print = false;
  bool saw_halt = false;

  for (size_t i = 0; i < instruction_index; ++i) {
    if (instructions[i] == kOpJumpIfFalse) {
      saw_jump_if_false = true;
    }
    if (instructions[i] == kOpJump) {
      saw_jump = true;
    }
    if (instructions[i] == kOpPrint) {
      saw_print = true;
    }
    if (instructions[i] == kOpHalt) {
      saw_halt = true;
    }
  }

  TEST_ASSERT_TRUE_MESSAGE(saw_jump_if_false, "Missing kOpJumpIfFalse for loop condition");
  TEST_ASSERT_TRUE_MESSAGE(saw_jump, "Missing kOpJump to loop start");
  TEST_ASSERT_TRUE_MESSAGE(saw_halt, "Missing kOpHalt at program end");

  TEST_ASSERT_FALSE_MESSAGE(saw_print, "Loop shouldn't run with false condition");
}