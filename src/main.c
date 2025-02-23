#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum Constants { kBufferSize = 81, kClearScreen = 147 };

int main() {
  printf("%c", kClearScreen);
  printf("Welcome to the Yap Language!\n");

  static char buffer[kBufferSize];

  int buffer_end_index = 0;
  static const char* const kExitToken = "run";
  const unsigned char kExitTokenLen = strlen(kExitToken);

  while (true) {
    putchar('>');
    putchar(' ');
    buffer_end_index = 0;

    while (true) {
      const int kInput = getchar();

      if (EOF == kInput) {
        break;
      }

      putchar(kInput);

      if ('\n' == kInput) {
        break;
      }

      buffer[buffer_end_index++] = kInput;
      buffer[buffer_end_index] = 0;

      if (buffer_end_index == sizeof(buffer) - 1) {
        break;
      }
    }

    if (buffer_end_index >= kExitTokenLen &&
        strncmp(kExitToken, buffer, kExitTokenLen) == 0) {
      break;
    }
  }

  return EXIT_SUCCESS;
}
