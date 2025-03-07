#include "vm.h"

#include <stdio.h>
#include <string.h>

typedef enum ValueType {
  kNumber,
  kString,
  kBool,
} ValueType;

// NOLINTBEGIN(cppcoreguidelines-avoid-non-const-global-variables)
static constexpr int kMaximumNumberOfOpcodes = 128;
unsigned char opcodes[kMaximumNumberOfOpcodes];
size_t opcode_index = 0;

static constexpr int kMaximumNumberOfConstants = 16;
static void* constants[kMaximumNumberOfConstants];
static ValueType constant_types[kMaximumNumberOfConstants];
static size_t constants_index = 0;

static constexpr int kStringPoolSize = 128;
static char string_pool[kStringPoolSize];
static size_t string_pool_index = 0;

static constexpr int kNumberPoolSize = 16;
static long number_pool[kNumberPoolSize];
static size_t number_pool_index = 0;

static constexpr int kBoolPoolSize = 16;
static bool bool_pool[kBoolPoolSize];
static size_t bool_pool_index = 0;

static constexpr int kStackSize = 8;
static size_t stack[kStackSize];
static size_t stack_index = 0;
// NOLINTEND(cppcoreguidelines-avoid-non-const-global-variables)

static void Push(const size_t value) {
  stack[stack_index++] = value;
}

static size_t Pop() {
  return stack[--stack_index];
}

void EmitByte(const unsigned char byte) { opcodes[opcode_index++] = byte; }

size_t AddBoolConstant(const bool boolean) {
  if (bool_pool_index +1 >= kBoolPoolSize) {
    return -1;
  }

  bool_pool[bool_pool_index] = boolean;

  constants[constants_index] = &bool_pool[bool_pool_index];
  constant_types[constants_index] = kBool;

  bool_pool_index++;

  return constants_index++;
}

size_t AddNumberConstant(const long number) {
  if (number_pool_index + 1 >= kNumberPoolSize) {
    return -1;
  }

  number_pool[number_pool_index] = number;

  constants[constants_index] = &number_pool[number_pool_index];
  constant_types[constants_index] = kNumber;

  number_pool_index++;

  return constants_index++;
}

size_t AddStringConstant(const char* const string) {
  const size_t kStringLength = strlen(string);

  if (string_pool_index + kStringLength + 1 > sizeof(string_pool)) {
    return -1;
  }

  char* const kCurrentStringPoolPointer = &string_pool[string_pool_index];

  // NOLINTNEXTLINE(clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling)
  strncpy(kCurrentStringPoolPointer, string, kStringLength);

  constants[constants_index] = kCurrentStringPoolPointer;
  constant_types[constants_index] = kString;

  string_pool_index += kStringLength + 1;

  return constants_index++;
}

void PrintOpcodes() {
  static constexpr int kRowLength = 8;

  if (0 == opcode_index) {
    printf("No opcodes.\n");

    return;
  }

  for (size_t index = 0; index < opcode_index; index++) {
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
      case kOpGreaterThan: {
        const size_t kFirstTerm = Pop();
        const size_t kSecondTerm = Pop();

        const size_t kResultIndex = AddBoolConstant(*(long*)constants[kSecondTerm] > *(long*)constants[kFirstTerm]);
        Push(kResultIndex);

        break;
      }
      case kOpPrint: {
        const unsigned char kIndex = Pop();

        if (kBool == constant_types[kIndex]) {
          printf("%s\n", *(bool*)constants[kIndex] ? "true" : "false");
          break;
        }

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
