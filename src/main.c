#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parser.h"

enum Constants { kBufferSize = 81, kClearScreen = 147 };

int main() {
  printf("%c", kClearScreen);
  printf("Welcome to the Yap Language!\n");

  static char buffer[kBufferSize];

  int buffer_end_index = 0;
  static const char* const kExitToken = "run";
  const unsigned int kExitTokenLen = strlen(kExitToken);

  while (true) {
    putchar('>');
    putchar(' ');
    buffer_end_index = 0;

    while (true) {
      const int kInputCharacter = getchar();

      if (EOF == kInputCharacter) {
        break;
      }

      if ('\n' == kInputCharacter) {
        break;
      }

      buffer[buffer_end_index++] = (char)kInputCharacter;
      buffer[buffer_end_index] = 0;

      if (buffer_end_index == sizeof(buffer) - 1) {
        break;
      }
    }

    if (buffer_end_index >= kExitTokenLen &&
        strncmp(kExitToken, buffer, kExitTokenLen) == 0) {
      break;
    }

    ParseProgram(buffer);
  }

  return EXIT_SUCCESS;
}
