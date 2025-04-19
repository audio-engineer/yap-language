#include "vm.h"

#ifdef __CC65__
#include <stdbool.h>
#endif
#include <stdio.h>
#include <string.h>

#ifdef __CC65__
enum {
  kCallFrameTableSize = 64,
  kConstantsSize = 128,
  kStringPoolSize = 512,
  kNumberPoolSize = 64,
  kStackSize = 16,
  kFunctionPoolSize = 16
};
#else
static constexpr int kCallFrameTableSize = 64;
static constexpr int kConstantsSize = 128;
static constexpr int kStringPoolSize = 512;
static constexpr int kNumberPoolSize = 64;
static constexpr int kStackSize = 16;
static constexpr int kFunctionPoolSize = 16;
#endif

/// Pushes a value onto the stack.
/// Is defined as a macro since cc65 doesn't support passing structs to
/// functions by value for regular functions.
#define Push(value) (stack[stack_index++] = (value))

/// Pops a value from the stack.
/// Is defined as a macro since cc65 doesn't support passing structs to
/// functions by value for regular functions.
#define Pop() (stack[--stack_index])

typedef struct Constants {
  const void* pointer[kConstantsSize];
  ConstantType type[kConstantsSize];
} Constants;

typedef struct Function {
  size_t instruction_address;
  size_t arity;
  VariableType return_type;
} Function;

typedef struct StackValue {
  VariableType type;
  union as {
    int number;
    char* string;
    Function* function;
  } as;
} StackValue;

typedef struct CallFrame {
  size_t return_address[64];
  size_t stack_offset[64];
  size_t arity[64];
} CallFrame;

// NOLINTBEGIN(cppcoreguidelines-avoid-non-const-global-variables)
static StackValue global_variables[64];
size_t global_variable_index = 0;

static CallFrame call_frames;
static size_t call_frame_index = 0;

unsigned char instructions[kInstructionsSize];
size_t instruction_index = 0;

static Constants constants;
size_t constants_index = 0;

static char string_pool[kStringPoolSize];
static size_t string_pool_index = 0;

static int number_pool[kNumberPoolSize];
static size_t number_pool_index = 0;

static Function function_pool[kFunctionPoolSize];
static size_t function_pool_index = 0;

static StackValue stack[kStackSize];
static size_t stack_index = 0;
// NOLINTEND(cppcoreguidelines-avoid-non-const-global-variables)

void ResetInterpreterState() {
  // NOLINTBEGIN(clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling)
  memset(&call_frames.return_address, 0, kCallFrameTableSize);
  memset(&call_frames.arity, 0, kCallFrameTableSize);
  memset(&call_frames.stack_offset, 0, kCallFrameTableSize);
  memset(instructions, 0, kInstructionsSize);
  memset(&constants.pointer, 0, kConstantsSize);
  memset(constants.type, 0, kConstantsSize);
  memset(string_pool, 0, kStringPoolSize);
  memset(number_pool, 0, kNumberPoolSize);
  memset(function_pool, 0, kFunctionPoolSize);
  memset(stack, 0, kStackSize);
  // NOLINTEND(clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling)

  call_frame_index = 0;
  instruction_index = 0;
  constants_index = 0;
  string_pool_index = 0;
  number_pool_index = 0;
  function_pool_index = 0;
  stack_index = 0;
}

void EmitByte(const unsigned char byte) {
  instructions[instruction_index++] = byte;
}

void EmitHalt() {
  if (kOpHalt != instructions[instruction_index - 1]) {
    EmitByte(kOpHalt);
  }
}

void RemoveHalt() {
  if (kOpHalt == instructions[instruction_index - 1]) {
    instructions[instruction_index--] = 0;
  }
}

// NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
size_t AddNumberConstant(const int value, const ConstantType constant_type) {
  // TODO(Martin): Add check for number_pool overflow.
  // TODO(Martin): Can't return -1 due to size_t return type
  // if (number_pool_index + 1 >= kNumberPoolSize) {
  //   return -1;
  // }

  number_pool[number_pool_index] = value;

  constants.pointer[constants_index] = &number_pool[number_pool_index];
  constants.type[constants_index] = constant_type;

  ++number_pool_index;

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
  constants.type[constants_index] = kConstantTypeString;

  string_pool_index += kStringLength + 1;

  return constants_index++;
}

// NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
size_t AddFunctionConstant(const size_t instruction_address,
                           const unsigned char parameter_count) {
  function_pool[function_pool_index].instruction_address = instruction_address;
  function_pool[function_pool_index].arity = parameter_count;

  constants.pointer[constants_index] = &function_pool[function_pool_index];
  constants.type[constants_index] = kConstantTypeFunction;

  ++function_pool_index;

  return constants_index++;
}

static void PushCallFrame(const Function* const function) {
  ++call_frame_index;

  call_frames.return_address[call_frame_index] = instruction_index;
  call_frames.arity[call_frame_index] = function->arity;
  call_frames.stack_offset[call_frame_index] =
      stack_index - function->arity - 1;

  instruction_index = function->return_type;
}

static void PopCallFrame(const StackValue* const stack_value) {
  stack_index = call_frames.stack_offset[call_frame_index];
  Push(*stack_value);
  instruction_index = call_frames.return_address[call_frame_index];

  --call_frame_index;
}

void PrintOpcodes() {
#ifdef __CC65__
  static const int kRowLength = 8;
#else
  static constexpr int kRowLength = 8;
#endif

  size_t index = 0;

  if (0 == instruction_index) {
    puts("No opcodes.");

    return;
  }

  for (index = 0; index < instruction_index; ++index) {
    if (0 != index && 0 == index % kRowLength) {
      puts("");
    }

    if (0 != index && 0 != index % kRowLength) {
      putchar(' ');
    }

    printf("%d", instructions[index]);
  }

  puts("");
}

// NOLINTNEXTLINE(readability-function-cognitive-complexity)
void RunVm() {
  instruction_index = 0;

  while (true) {
    const Opcode kOpcode = instructions[instruction_index++];

    switch (kOpcode) {
      case kOpConstant: {
        const size_t kIndex = instructions[instruction_index++];

        StackValue stack_value = {};
        stack_value.type = constants.type[kIndex];

        if (kConstantTypeString == constants.type[kIndex]) {
          stack_value.as.string = (char*)constants.pointer[kIndex];
        } else {
          stack_value.as.number = *(const int*)constants.pointer[kIndex];
        }

        Push(stack_value);

        break;
      }
      case kOpAdd: {
        StackValue stack_value_one = {};
        StackValue stack_value_two = {};
        StackValue result = {};

        stack_value_one = Pop();
        stack_value_two = Pop();

        result.as.number =
            stack_value_two.as.number + stack_value_one.as.number;
        result.type = kConstantTypeNumber;

        Push(result);

        break;
      }
      case kOpSubtract: {
        StackValue stack_value_one = {};
        StackValue stack_value_two = {};
        StackValue result = {};

        stack_value_one = Pop();
        stack_value_two = Pop();

        result.as.number =
            stack_value_two.as.number - stack_value_one.as.number;
        result.type = kConstantTypeNumber;

        Push(result);

        break;
      }
      case kOpMultiply: {
        StackValue stack_value_one = {};
        StackValue stack_value_two = {};
        StackValue result = {};

        stack_value_one = Pop();
        stack_value_two = Pop();

        result.as.number =
            stack_value_two.as.number * stack_value_one.as.number;
        result.type = kConstantTypeNumber;

        Push(result);

        break;
      }
      case kOpDivide: {
        StackValue stack_value_one = {};
        StackValue stack_value_two = {};
        StackValue result = {};

        stack_value_one = Pop();
        stack_value_two = Pop();

        if (0 == stack_value_one.as.number) {
          puts("Error: Division by zero.");

          break;
        }

        result.as.number =
            stack_value_two.as.number / stack_value_one.as.number;
        result.type = kConstantTypeNumber;

        Push(result);

        break;
      }
      case kOpEquals: {
        StackValue stack_value_one = {};
        StackValue stack_value_two = {};
        StackValue result = {};

        stack_value_one = Pop();
        stack_value_two = Pop();

        result.as.number =
            stack_value_two.as.number == stack_value_one.as.number;
        result.type = kConstantTypeBoolean;

        Push(result);

        break;
      }
      case kOpNotEquals: {
        StackValue stack_value_one = {};
        StackValue stack_value_two = {};
        StackValue result = {};

        stack_value_one = Pop();
        stack_value_two = Pop();

        result.as.number =
            stack_value_two.as.number != stack_value_one.as.number;
        result.type = kConstantTypeBoolean;

        Push(result);

        break;
      }
      case kOpGreaterThan: {
        StackValue stack_value_one = {};
        StackValue stack_value_two = {};
        StackValue result = {};

        stack_value_one = Pop();
        stack_value_two = Pop();

        result.as.number =
            stack_value_two.as.number > stack_value_one.as.number;
        result.type = kConstantTypeBoolean;

        Push(result);

        break;
      }
      case kOpGreaterThanOrEqualTo: {
        StackValue stack_value_one = {};
        StackValue stack_value_two = {};
        StackValue result = {};

        stack_value_one = Pop();
        stack_value_two = Pop();

        result.as.number =
            stack_value_two.as.number >= stack_value_one.as.number;
        result.type = kConstantTypeBoolean;

        Push(result);

        break;
      }
      case kOpLessThan: {
        StackValue stack_value_one = {};
        StackValue stack_value_two = {};
        StackValue result = {};

        stack_value_one = Pop();
        stack_value_two = Pop();

        result.as.number =
            stack_value_two.as.number < stack_value_one.as.number;
        result.type = kConstantTypeBoolean;

        Push(result);

        break;
      }
      case kOpLessThanOrEqualTo: {
        StackValue stack_value_one = {};
        StackValue stack_value_two = {};
        StackValue result = {};

        stack_value_one = Pop();
        stack_value_two = Pop();

        result.as.number =
            stack_value_two.as.number <= stack_value_one.as.number;
        result.type = kConstantTypeBoolean;

        Push(result);

        break;
      }
      case kOpPrint: {
        StackValue stack_value = {};
        stack_value = Pop();

        switch (stack_value.type) {
          case kConstantTypeString:
            printf("%s\n", stack_value.as.string);

            break;
          case kConstantTypeNumber:
            printf("%d\n", stack_value.as.number);

            break;
          case kConstantTypeBoolean:
            printf("%s\n", stack_value.as.number ? "true" : "false");

            break;
          default:
            puts("Error: Unknown print type.");

            break;
        }

        break;
      }
      case kOpJumpIfFalse: {
        const size_t kJumpAddress = instructions[instruction_index++];
        StackValue stack_value = {};

        stack_value = Pop();

        if (kConstantTypeBoolean == stack_value.type &&
            0 == stack_value.as.number) {
          instruction_index = kJumpAddress;
        }

        break;
      }
      case kOpJump: {
        const size_t kJumpAddress = instructions[instruction_index++];

        instruction_index = kJumpAddress;

        break;
      }
      case kOpStoreGlobal: {
        const size_t kGlobalVariablesIndex = instructions[instruction_index++];

        // TODO(Martin): Implement type checker.
        // NOLINTNEXTLINE(clang-analyzer-deadcode.DeadStores)
        const VariableType kGlobalVariableType =
            instructions[instruction_index++];
        StackValue stack_value = {};

        stack_value = Pop();

        global_variables[kGlobalVariablesIndex] = stack_value;

        break;
      }
      case kOpLoadGlobal: {
        const size_t kGlobalVariablesIndex = instructions[instruction_index++];

        // TODO(Martin): Implement type checker.
        // NOLINTNEXTLINE(clang-analyzer-deadcode.DeadStores)
        const VariableType kGlobalVariableType =
            instructions[instruction_index++];
        StackValue stack_value = {};
        stack_value = global_variables[kGlobalVariablesIndex];

        Push(stack_value);

        break;
      }
      case kOpStoreLocal: {
        const size_t kIndex = instructions[instruction_index++];

        stack[kIndex] = Pop();

        break;
      }
      case kOpLoadLocal: {
        const size_t kGlobalVariablesIndex = instructions[instruction_index++];

        Push(stack[kGlobalVariablesIndex]);

        break;
      }
      case kOpDefineFunction: {
        const size_t kIndex = instructions[instruction_index++];
        const size_t kStart = instructions[instruction_index++];
        const size_t kArity = instructions[instruction_index++];
        const VariableType kReturnType = instructions[instruction_index++];
        StackValue stack_value = {};

        function_pool[function_pool_index].instruction_address = kStart;
        function_pool[function_pool_index].arity = kArity;
        function_pool[function_pool_index].return_type = kReturnType;

        stack_value.as.function = &function_pool[function_pool_index];
        global_variables[kIndex] = stack_value;

        break;
      }
      case kOpCallFunction: {
        const size_t kArity = instructions[instruction_index++];
        const size_t kFunctionIndex = stack_index - kArity - 1;
        StackValue stack_value = {};

        stack_value = stack[kFunctionIndex];

        PushCallFrame(stack_value.as.function);

        break;
      }
      case kOpReturn: {
        StackValue stack_value = {};

        stack_value = Pop();

        PopCallFrame(&stack_value);

        break;
      }
      case kOpPushCallFrame: {
        instruction_index += 2;

        break;
      }
      case kOpPopCallFrame: {
        break;
      }
      case kOpHalt: {
        return;
      }
      default: {
        printf("Error: Undefined opcode '%d'.\n", kOpcode);

        return;
      }
    }
  }
}
