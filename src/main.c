#ifdef __CC65__
#include <stdbool.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lexer.h"
#include "parser.h"
#include "vm.h"

#ifdef __CC65__
enum { kClearScreen = 147, kLineBufferSize = 81 };
#else
static constexpr int kLineBufferSize = 81;
#endif

typedef enum ExecutionMode { kModeDirect, kModeProgram } ExecutionMode;

// NOLINTBEGIN(cppcoreguidelines-avoid-non-const-global-variables)
static ExecutionMode current_mode = kModeDirect;
static char line_buffer[kLineBufferSize];
static size_t line_buffer_length = 0;
// NOLINTEND(cppcoreguidelines-avoid-non-const-global-variables)

static void PrintHelp() {
  puts("Usage:");
  puts("help  Show this message.");
  puts("ops   Print opcodes currently in buffer.");
  puts("clear Clear the program buffer.");
  puts("exit  Exit the interpreter.");
  puts("Direct mode:");
  puts("prog  Enter program mode.");
  puts("Program mode:");
  puts("run   Run entire program.");
  puts("cont  Run program.");
  puts("dir   Return to direct mode.");
}

static void PrintMode(const char* const mode) {
  puts("");
  printf("%s mode.\n", mode);
}

static void CompileProgram() {
  program_buffer_index = 0;

  ResetInterpreterState();
  ParseProgram();
  EmitHalt();
}

static void DirectMode() {
  ResetLexerState();

  line_buffer_length = strlen(line_buffer);

  // NOLINTNEXTLINE(clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling)
  strncpy(&program_buffer[program_buffer_index], line_buffer,
          line_buffer_length);
  program_buffer[line_buffer_length] = '\0';

  CompileProgram();
  RunVm();
}

static void ProgramMode() {
  if (0 == strncmp("cont", line_buffer, 4)) {
    RunVm();

    return;
  }

  if (0 == strncmp("run", line_buffer, 3)) {
    CompileProgram();
    RunVm();

    return;
  }

  RemoveHalt();

  line_buffer_length = strlen(line_buffer);

  if (kProgramBufferSize <= program_buffer_index + line_buffer_length + 1) {
    puts("Error: Program buffer overflow.");

    return;
  }

  // NOLINTNEXTLINE(clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling)
  strncpy(&program_buffer[program_buffer_index], line_buffer,
          line_buffer_length);

  program_buffer_index += line_buffer_length;

  program_buffer[program_buffer_index] = '\0';
}

int main() {
#ifdef __CC65__
  putchar(kClearScreen);
#endif

  puts("Welcome to the Yap Language!");
  puts("");
  puts("Run 'help' to see a list of commands.");
  PrintMode("Direct");

  while (true) {
    // NOLINTNEXTLINE(clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling)
    memset(line_buffer, 0, kLineBufferSize);

    if (kModeDirect == current_mode) {
      printf("> ");
    }

    if (!fgets(line_buffer, kLineBufferSize, stdin)) {
      break;
    }

    if (0 == strncmp("exit", line_buffer, 4)) {
      break;
    }

    // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
    if (0 == strncmp(line_buffer, "clear", 5)) {
      ResetLexerState();
      ResetInterpreterState();

      puts("Cleared.");

      continue;
    }

    if (0 == strncmp("help", line_buffer, 4)) {
      PrintHelp();

      continue;
    }

    if (0 == strncmp("ops", line_buffer, 3)) {
      PrintOpcodes();

      continue;
    }

    if (0 == strncmp("prog", line_buffer, 4) && kModeDirect == current_mode) {
      ResetLexerState();
      ResetInterpreterState();

      current_mode = kModeProgram;

      PrintMode("Program");

      continue;
    }

    if (0 == strncmp("dir", line_buffer, 3) && kModeProgram == current_mode) {
      ResetLexerState();
      ResetInterpreterState();

      current_mode = kModeDirect;

      PrintMode("Direct");

      continue;
    }

    if (kModeDirect == current_mode) {
      DirectMode();

      continue;
    }

    if (kModeProgram == current_mode) {
      ProgramMode();
    }
  }

  puts("Bye!");

  return EXIT_SUCCESS;
}
