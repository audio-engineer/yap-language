#include "vm.h"

#include <stdio.h>
#include <string.h>

#ifdef COMMODORE
enum Constants {
  kOpcodesSize = 128,
  kConstantsSize = 128,
  kStringPoolSize = 512,
  kNumberPoolSize = 64,
  kStackSize = 16,
};
#else
static constexpr int kOpcodesSize = 128;
static constexpr int kConstantsSize = 128;
static constexpr int kStringPoolSize = 512;
static constexpr int kNumberPoolSize = 64;
static constexpr int kStackSize = 16;
#endif

typedef enum ValueType {
  kNumber,
  kString,
} ValueType;

// NOLINTBEGIN(cppcoreguidelines-avoid-non-const-global-variables)
unsigned char opcodes[kOpcodesSize];
size_t opcode_index = 0;

static void* constants[kConstantsSize];
static ValueType constant_types[kConstantsSize];
static size_t constants_index = 0;

static char string_pool[kStringPoolSize];
static size_t string_pool_index = 0;

static long number_pool[kNumberPoolSize];
static size_t number_pool_index = 0;

static size_t stack[kStackSize];
static size_t stack_index = 0;
// NOLINTEND(cppcoreguidelines-avoid-non-const-global-variables)

static void Push(const size_t value) { stack[stack_index++] = value; }

static size_t Pop() { return stack[--stack_index]; }

void EmitByte(const unsigned char byte) { opcodes[opcode_index++] = byte; }

size_t AddNumberConstant(const long number) {
  // TODO(Martin): Add check for number_pool overflow.
  // TODO(Martin): Can't return -1 due to size_t return type
  // if (number_pool_index + 1 >= kNumberPoolSize) {
  //   return -1;
  // }

  number_pool[number_pool_index] = number;

  constants[constants_index] = &number_pool[number_pool_index];
  constant_types[constants_index] = kNumber;

  number_pool_index++;

  return constants_index++;
}

size_t AddStringConstant(const char* const string) {
  const size_t kStringLength = strlen(string);
#ifdef COMMODORE
  char* current_string_pool_pointer = NULL;
#else
  char* current_string_pool_pointer = nullptr;
#endif

  // TODO(Martin): Add check for string_pool overflow
  // TODO(Martin): Can't return -1 due to size_t return type
  // if (string_pool_index + kStringLength + 1 > kStringPoolSize) {
  //   return -1;
  // }

  current_string_pool_pointer = &string_pool[string_pool_index];

  // NOLINTNEXTLINE(clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling)
  strncpy(current_string_pool_pointer, string, kStringLength);

  constants[constants_index] = current_string_pool_pointer;
  constant_types[constants_index] = kString;

  string_pool_index += kStringLength + 1;

  return constants_index++;
}

void PrintOpcodes() {
#ifdef COMMODORE
  static const int kRowLength = 8;
#else
  static constexpr int kRowLength = 8;
#endif

  size_t index = 0;

  if (0 == opcode_index) {
    printf("No opcodes.\n");

    return;
  }

  for (index = 0; index < opcode_index; index++) {
    if (0 != index && 0 == index % kRowLength) {
      printf("\n");
    }

    if (0 != index && 0 != index % kRowLength) {
      printf(" ");
    }

    printf("%d", opcodes[index]);
  }

  printf("\n");
}

void RunVm() {
  int instruction = 0;

  while (1) {
    const unsigned char kOpCode = opcodes[instruction++];

    switch (kOpCode) {
      case kOpConstant: {
        const unsigned char kIndex = opcodes[instruction++];

        Push(kIndex);

        break;
      }
      case kOpAdd: {
        const size_t kFirstTerm = Pop();
        const size_t kSecondTerm = Pop();

        const size_t kResultIndex = AddNumberConstant(
            *(long*)constants[kFirstTerm] + *(long*)constants[kSecondTerm]);

        Push(kResultIndex);

        break;
      }
      case kOpSubtract: {
        const size_t kFirstTerm = Pop();
        const size_t kSecondTerm = Pop();

        const size_t kResultIndex = AddNumberConstant(
            *(long*)constants[kSecondTerm] - *(long*)constants[kFirstTerm]);

        Push(kResultIndex);

        break;
      }
      case kOpPrint: {
        const unsigned char kIndex = Pop();

        if (kNumber == constant_types[kIndex]) {
          printf("%ld\n", *(long*)constants[kIndex]);

          break;
        }

        if (kString == constant_types[kIndex]) {
          printf("%s\n", (const char* const)constants[kIndex]);

          break;
        }

        printf("Unknown print type.");

        break;
      }
      case kOpIf: {
        printf("If statement is not implemented\n");

        break;
      }
      case kOpHalt: {
        return;
      }
      default: {
        printf("Undefined opcode '%c'\n", kOpCode);

        return;
      }
    }
  }
}
