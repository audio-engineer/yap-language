#include "vm.h"

#ifdef __CC65__
#include <stdbool.h>
#endif
#include <stdio.h>
#include <string.h>

// NOLINTBEGIN(cppcoreguidelines-avoid-non-const-global-variables)
unsigned char opcodes[kOpcodesSize];
size_t opcode_index = 0;

Constants constants;
size_t constants_index = 0;

char string_pool[kStringPoolSize];
size_t string_pool_index = 0;

long number_pool[kNumberPoolSize];
size_t number_pool_index = 0;

size_t stack[kStackSize];
size_t stack_index = 0;
// NOLINTEND(cppcoreguidelines-avoid-non-const-global-variables)

void ResetInterpreterState() {
  // NOLINTBEGIN(clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling)
  memset(opcodes, 0, kOpcodesSize);
  memset(&constants.pointer, 0, kConstantsSize);
  memset(constants.type, 0, kConstantsSize);
  memset(string_pool, 0, kStringPoolSize);
  memset(number_pool, 0, kNumberPoolSize);
  memset(stack, 0, kStackSize);
  // NOLINTEND(clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling)

  opcode_index = 0;
  constants_index = 0;
  string_pool_index = 0;
  number_pool_index = 0;
  stack_index = 0;
}

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
    puts("No opcodes.");

    return;
  }

  for (index = 0; index < opcode_index; index++) {
    if (0 != index && 0 == index % kRowLength) {
      puts("");
    }

    if (0 != index && 0 != index % kRowLength) {
      printf(" ");
    }

    printf("%d", opcodes[index]);
  }

  puts("");
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
          puts("Error: Division by zero.");

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

        puts("Error: Unknown print type.");

        break;
      }
      case kOpIf: {
        puts("Error: If statement is not implemented.");

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
