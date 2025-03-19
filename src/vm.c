#include "vm.h"

#ifdef __CC65__
#include <stdbool.h>
#endif
#include <stdio.h>
#include <string.h>

#ifdef __CC65__
enum {
  kOpcodesSize = 128,
  kConstantsSize = 128,
  kStringPoolSize = 512,
  kNumberPoolSize = 64,
  kStackSize = 16
};
#else
static constexpr int kOpcodesSize = 128;
static constexpr int kConstantsSize = 128;
static constexpr int kStringPoolSize = 512;
static constexpr int kNumberPoolSize = 64;
static constexpr int kStackSize = 16;
#endif

// NOLINTBEGIN(cppcoreguidelines-avoid-non-const-global-variables)
unsigned char opcodes[kOpcodesSize];
size_t opcode_index = 0;

typedef struct Constants {
  const void* pointer[kConstantsSize];
  ConstantType type[kConstantsSize];
} Constants;
static Constants constants;
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

// NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
size_t AddNumberConstant(const long value, const ConstantType constant_type) {
  // TODO(Martin): Add check for number_pool overflow.
  // TODO(Martin): Can't return -1 due to size_t return type
  // if (number_pool_index + 1 >= kNumberPoolSize) {
  //   return -1;
  // }

  number_pool[number_pool_index] = value;

  constants.pointer[constants_index] = &number_pool[number_pool_index];
  constants.type[constants_index] = constant_type;

  number_pool_index++;

  return constants_index++;
}

size_t AddStringConstant(const char* const string) {
  const size_t kStringLength = strlen(string);
#ifdef __CC65__
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

  constants.pointer[constants_index] = current_string_pool_pointer;
  constants.type[constants_index] = kTypeString;

  string_pool_index += kStringLength + 1;

  return constants_index++;
}

void PrintOpcodes() {
#ifdef __CC65__
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

  while (true) {
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

        const size_t kResultIndex =
            AddNumberConstant(*(const long*)constants.pointer[kFirstTerm] +
                                  *(const long*)constants.pointer[kSecondTerm],
                              kTypeNumber);

        Push(kResultIndex);

        break;
      }
      case kOpSubtract: {
        const size_t kFirstTerm = Pop();
        const size_t kSecondTerm = Pop();

        const size_t kResultIndex =
            AddNumberConstant(*(const long*)constants.pointer[kSecondTerm] -
                                  *(const long*)constants.pointer[kFirstTerm],
                              kTypeNumber);

        Push(kResultIndex);

        break;
      }
      case kOpMultiply: {
        const size_t kFirstTerm = Pop();
        const size_t kSecondTerm = Pop();

        const size_t kResultIndex =
            AddNumberConstant(*(const long*)constants.pointer[kSecondTerm] *
                                  *(const long*)constants.pointer[kFirstTerm],
                              kTypeNumber);

        Push(kResultIndex);

        break;
      }
      case kOpDivide: {
        const size_t kFirstTerm = Pop();
        const size_t kSecondTerm = Pop();

        size_t result_index = 0;

        if (0 == *(const long*)constants.pointer[kFirstTerm]) {
          puts("Error: Division by zero");

          return;
        }

        result_index =
            AddNumberConstant(*(const long*)constants.pointer[kSecondTerm] /
                                  *(const long*)constants.pointer[kFirstTerm],
                              kTypeNumber);

        Push(result_index);
        break;
      }
      case kOpGreaterThan: {
        const size_t kFirstTerm = Pop();
        const size_t kSecondTerm = Pop();

        const size_t kResultIndex = AddNumberConstant(
            0 != (*(const long*)constants.pointer[kSecondTerm] >
                  *(const long*)constants.pointer[kFirstTerm]),
            kTypeBoolean);
        Push(kResultIndex);

        break;
      }
      case kOpGreaterOrEquals: {
        const size_t kFirstTerm = Pop();
        const size_t kSecondTerm = Pop();

        const size_t kResultIndex = AddNumberConstant(
            0 != (*(const long*)constants.pointer[kSecondTerm] >=
                  *(const long*)constants.pointer[kFirstTerm]),
            kTypeBoolean);
        Push(kResultIndex);

        break;
      }
      case kOpLessOrEquals: {
        const size_t kFirstTerm = Pop();
        const size_t kSecondTerm = Pop();

        const size_t kResultIndex = AddNumberConstant(
            0 != (*(const long*)constants.pointer[kSecondTerm] <=
                  *(const long*)constants.pointer[kFirstTerm]),
            kTypeBoolean);
        Push(kResultIndex);

        break;
      }
      case kOpLessThan: {
        const size_t kFirstTerm = Pop();
        const size_t kSecondTerm = Pop();

        const size_t kResultIndex = AddNumberConstant(
            0 != (*(const long*)constants.pointer[kSecondTerm] <
                  *(const long*)constants.pointer[kFirstTerm]),
            kTypeBoolean);
        Push(kResultIndex);

        break;
      }
      case kOpPrint: {
        const unsigned char kIndex = Pop();

        if (kTypeBoolean == constants.type[kIndex]) {
          printf("%s\n",
                 *(const int*)constants.pointer[kIndex] ? "true" : "false");

          break;
        }

        if (kTypeNumber == constants.type[kIndex]) {
          printf("%ld\n", *(const long*)constants.pointer[kIndex]);

          break;
        }

        if (kTypeString == constants.type[kIndex]) {
          printf("%s\n", (const char* const)constants.pointer[kIndex]);

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
