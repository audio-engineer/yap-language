#ifdef __CC65__
#include <stdbool.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parser.h"
#include "vm.h"

#ifdef __CC65__
enum Constants {
  kInputBufferSize = 81,
  kClearScreen = 147,
};
#else
static constexpr int kInputBufferSize = 81;
#endif

void PrintHelp() {
  printf("Usage:\n");
  printf("run   Run your program.\n");
  printf("ops   Print opcodes currently in buffer.\n");
  printf("exit  Exit the interpreter.\n");
  printf("help  Show this message.\n");
}

int main() {
  static char input_buffer[kInputBufferSize];

#ifdef __CC65__
  putchar(kClearScreen);
#endif

  printf("Welcome to the Yap Language!\n");
  printf("\n");
  printf("Run 'help' to see a list of commands.\n");

  while (true) {
    printf("> ");

    // NOLINTNEXTLINE(clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling)
    memset(input_buffer, 0, kInputBufferSize);
    fgets(input_buffer, kInputBufferSize, stdin);

    if (0 == strncmp("exit", input_buffer, 4)) {
      break;
    }

    if (0 == strncmp("help", input_buffer, 4)) {
      PrintHelp();

      continue;
    }

    if (0 == strncmp("ops", input_buffer, 3)) {
      PrintOpcodes();

      continue;
    }

    if (0 == strncmp("run", input_buffer, 3)) {
      // If the program hasn't been run before, add halt opcode
      if (kOpHalt != opcodes[opcode_index - 1]) {
        EmitByte(kOpHalt);
      }

      RunVm();

      continue;
    }

    // If the program has been run before, remove halt opcode
    if (kOpHalt == opcodes[opcode_index - 1]) {
      opcodes[opcode_index--] = 0;
    }

    ParseProgram(input_buffer);
  }

  printf("Bye!\n");

  return EXIT_SUCCESS;
}
