#include "vm.h"

#ifdef __CC65__
#include <stdbool.h>
#endif
#include <stdio.h>
#include <string.h>

#ifdef __CC65__
enum {
  kGlobalVariablesSize = 64,
  kCallFrameTableSize = 64,
  kConstantsSize = 128,
  kStringPoolSize = 512,
  kNumberPoolSize = 64,
  kStackSize = 16,
  kFunctionPoolSize = 16,
  kArrayPoolSize = 16,
  kArrayElementsMax = 16
};
#else
static constexpr int kGlobalVariablesSize = 64;
static constexpr int kCallFrameTableSize = 64;
static constexpr int kConstantsSize = 128;
static constexpr int kStringPoolSize = 512;
static constexpr int kNumberPoolSize = 64;
static constexpr int kStackSize = 16;
static constexpr int kFunctionPoolSize = 16;
static constexpr int kArrayPoolSize = 16;
static constexpr int kArrayElementsMax = 16;

#endif

/// Pushes a value onto the stack.
/// Defined as a macro since cc65 doesn't support passing structs to functions
/// by value for regular functions.
#define Push(value)                   \
  do {                                \
    if (kStackSize <= stack_index) {  \
      puts("Error: Stack overflow."); \
    } else {                          \
      stack[stack_index++] = (value); \
    }                                 \
  } while (0)

/// Pops a value from the stack.
/// Defined as a macro since cc65 doesn't support passing structs to functions
/// by value for regular functions.
#define Pop()                                                             \
  (0 == stack_index ? (puts("Error: Stack underflow."), kEmptyStackValue) \
                    : stack[--stack_index])

typedef struct Constants {
  const void* pointer[kConstantsSize];
  ConstantType type[kConstantsSize];
} Constants;

// TODO(Martin): Convert to struct of arrays.
typedef struct Function {
  size_t body_start_index;
  size_t arity;
  VariableType return_type;
} Function;

typedef struct Array {
  int elements[kArrayElementsMax];
  size_t count;
} Array;

typedef struct StackValue {
  union as {
    int number;
    char* string;
    Function* function;
    Array* array;
  } as;
  VariableType type;
#ifdef __CC65__
  unsigned char padding;  ///< Used to add 1 byte padding to the struct, so that
                          ///< the whole struct has a size of exactly 4 bytes.
                          ///< See https://cc65.github.io/doc/cc65.html#s4
#endif
} StackValue;

static const StackValue kEmptyStackValue = {};

typedef struct CallFrame {
  size_t return_address[kCallFrameTableSize];
  size_t stack_offset[kCallFrameTableSize];
  size_t arity[kCallFrameTableSize];
} CallFrame;

// NOLINTBEGIN(cppcoreguidelines-avoid-non-const-global-variables)
static StackValue global_variables[kGlobalVariablesSize];
size_t global_variable_index = 0;

static CallFrame call_frames;
static size_t call_frame_index = 0;

unsigned char instructions[kInstructionsSize];
size_t instruction_address = 0;

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

static Array array_pool[kArrayPoolSize];
static size_t array_pool_index = 0;

// NOLINTEND(cppcoreguidelines-avoid-non-const-global-variables)

void ResetInterpreterState() {
  // NOLINTBEGIN(clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling)
  memset(global_variables, 0, kGlobalVariablesSize);
  memset(call_frames.return_address, 0, kCallFrameTableSize);
  memset(call_frames.arity, 0, kCallFrameTableSize);
  memset(call_frames.stack_offset, 0, kCallFrameTableSize);
  memset(instructions, 0, kInstructionsSize);
  memset(&constants.pointer, 0, kConstantsSize);
  memset(constants.type, 0, kConstantsSize);
  memset(string_pool, 0, kStringPoolSize);
  memset(number_pool, 0, kNumberPoolSize);
  memset(stack, 0, kStackSize * sizeof(StackValue));
  // NOLINTEND(clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling)

  global_variable_index = 0;
  call_frame_index = 0;
  instruction_address = 0;
  constants_index = 0;
  string_pool_index = 0;
  number_pool_index = 0;
  function_pool_index = 0;
  stack_index = 0;
}

void EmitByte(const unsigned char byte) {
  instructions[instruction_address++] = byte;
}

void EmitHalt() {
  if (kOpHalt != instructions[instruction_address - 1]) {
    EmitByte(kOpHalt);
  }
}

void RemoveHalt() {
  if (kOpHalt == instructions[instruction_address - 1]) {
    instructions[instruction_address--] = 0;
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

static void PushCallFrame(const Function* const function) {
  call_frames.return_address[call_frame_index] = instruction_address;
  call_frames.arity[call_frame_index] = function->arity;
  call_frames.stack_offset[call_frame_index] =
      stack_index - function->arity - 1;

  instruction_address = function->body_start_index;

  ++call_frame_index;
}

static void PopCallFrame(const StackValue* const stack_value) {
  --call_frame_index;

  stack_index = call_frames.stack_offset[call_frame_index];

  Push(*stack_value);

  instruction_address = call_frames.return_address[call_frame_index];
}

void PrintOpcodes() {
#ifdef __CC65__
  static const size_t kRowLength = 8;
#else
  static constexpr size_t kRowLength = 8;
#endif

  size_t index = 0;

  if (0 == instruction_address) {
    puts("No opcodes.");

    return;
  }

  for (index = 0; index < instruction_address; ++index) {
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
  instruction_address = 0;

  while (true) {
    const Opcode kOpcode = instructions[instruction_address++];

    switch (kOpcode) {
      case kOpConstant: {
        const size_t kIndex = instructions[instruction_address++];

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

        // cppcheck-suppress-begin redundantInitialization
        stack_value_one = Pop();
        stack_value_two = Pop();
        // cppcheck-suppress-end redundantInitialization

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

        // cppcheck-suppress-begin redundantInitialization
        stack_value_one = Pop();
        stack_value_two = Pop();
        // cppcheck-suppress-end redundantInitialization

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

        // cppcheck-suppress-begin redundantInitialization
        stack_value_one = Pop();
        stack_value_two = Pop();
        // cppcheck-suppress-end redundantInitialization

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

        // cppcheck-suppress-begin redundantInitialization
        stack_value_one = Pop();
        stack_value_two = Pop();
        // cppcheck-suppress-end redundantInitialization

        if (0 == stack_value_one.as.number) {
          puts("Error: Division by zero.");

          return;
        }

        result.as.number =
            stack_value_two.as.number / stack_value_one.as.number;
        result.type = kConstantTypeNumber;

        Push(result);

        break;
      }
      case kOpModulo: {
        StackValue stack_value_one = {};
        StackValue stack_value_two = {};
        StackValue result = {};

        // cppcheck-suppress-begin redundantInitialization
        stack_value_one = Pop();
        stack_value_two = Pop();
        // cppcheck-suppress-end redundantInitialization

        if (0 == stack_value_one.as.number) {
          puts("Error: Division by zero.");

          return;
        }

        result.as.number =
            stack_value_two.as.number % stack_value_one.as.number;
        result.type = kConstantTypeNumber;

        Push(result);

        break;
      }
      case kOpEquals: {
        StackValue stack_value_one = {};
        StackValue stack_value_two = {};
        StackValue result = {};

        // cppcheck-suppress-begin redundantInitialization
        stack_value_one = Pop();
        stack_value_two = Pop();
        // cppcheck-suppress-end redundantInitialization

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

        // cppcheck-suppress-begin redundantInitialization
        stack_value_one = Pop();
        stack_value_two = Pop();
        // cppcheck-suppress-end redundantInitialization

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

        // cppcheck-suppress-begin redundantInitialization
        stack_value_one = Pop();
        stack_value_two = Pop();
        // cppcheck-suppress-end redundantInitialization

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

        // cppcheck-suppress-begin redundantInitialization
        stack_value_one = Pop();
        stack_value_two = Pop();
        // cppcheck-suppress-end redundantInitialization

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

        // cppcheck-suppress-begin redundantInitialization
        stack_value_one = Pop();
        stack_value_two = Pop();
        // cppcheck-suppress-end redundantInitialization

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

        // cppcheck-suppress-begin redundantInitialization
        stack_value_one = Pop();
        stack_value_two = Pop();
        // cppcheck-suppress-end redundantInitialization

        result.as.number =
            stack_value_two.as.number <= stack_value_one.as.number;
        result.type = kConstantTypeBoolean;

        Push(result);

        break;
      }
      case kOpPrint: {
        StackValue stack_value = {};

        // cppcheck-suppress redundantInitialization
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
        const size_t kJumpAddress = instructions[instruction_address++];
        StackValue stack_value = {};

        // cppcheck-suppress redundantInitialization
        stack_value = Pop();

        if (kConstantTypeBoolean == stack_value.type &&
            0 == stack_value.as.number) {
          instruction_address = kJumpAddress;
        }

        break;
      }
      case kOpJump: {
        const size_t kJumpAddress = instructions[instruction_address++];

        instruction_address = kJumpAddress;

        break;
      }
      case kOpStoreGlobal: {
        const size_t kIndex = instructions[instruction_address++];

        // TODO(Martin): Implement type checker.
        // cppcheck-suppress unreadVariable
        // NOLINTNEXTLINE(clang-analyzer-deadcode.DeadStores)
        const VariableType kVariableType = instructions[instruction_address++];

        StackValue global_variable = {};

        // cppcheck-suppress redundantInitialization
        global_variable = Pop();

        global_variables[kIndex] = global_variable;

        break;
      }
      case kOpLoadGlobal: {
        const size_t kIndex = instructions[instruction_address++];

        // TODO(Martin): Implement type checker.
        // cppcheck-suppress unreadVariable
        // NOLINTNEXTLINE(clang-analyzer-deadcode.DeadStores)
        const VariableType kVariableType = instructions[instruction_address++];

        StackValue global_variable = {};

        // cppcheck-suppress redundantInitialization
        global_variable = global_variables[kIndex];

        Push(global_variable);

        break;
      }
      case kOpStoreLocal: {
        const size_t kIndex = instructions[instruction_address++];

        // TODO(Martin): Implement type checker.
        // cppcheck-suppress unreadVariable
        // NOLINTNEXTLINE(clang-analyzer-deadcode.DeadStores)
        const VariableType kVariableType = instructions[instruction_address++];

        const size_t kStackOffset =
            call_frames.stack_offset[call_frame_index - 1];

        stack[kStackOffset + 1 + kIndex] = Pop();

        break;
      }
      case kOpLoadLocal: {
        const size_t kIndex = instructions[instruction_address++];

        // TODO(Martin): Implement type checker.
        // cppcheck-suppress unreadVariable
        // NOLINTNEXTLINE(clang-analyzer-deadcode.DeadStores)
        const VariableType kVariableType = instructions[instruction_address++];

        const size_t kStackOffset =
            call_frames.stack_offset[call_frame_index - 1];

        Push(stack[kStackOffset + 1 + kIndex]);

        break;
      }
      case kOpDefineFunction: {
        const size_t kIndex = instructions[instruction_address++];
        const size_t kBodyStartIndex = instructions[instruction_address++];
        const size_t kArity = instructions[instruction_address++];
        const VariableType kReturnType = instructions[instruction_address++];

        StackValue global_variable = {};

        Function* function = &function_pool[function_pool_index];
        function->body_start_index = kBodyStartIndex;
        function->arity = kArity;
        function->return_type = kReturnType;

        ++function_pool_index;

        global_variable.type = kConstantTypeFunction;
        global_variable.as.function = function;

        global_variables[kIndex] = global_variable;

        break;
      }
      case kOpCallFunction: {
        const size_t kArity = instructions[instruction_address++];
        const size_t kFunctionIndex = stack_index - kArity - 1;
        StackValue stack_value = {};

        // cppcheck-suppress redundantInitialization
        stack_value = stack[kFunctionIndex];

        PushCallFrame(stack_value.as.function);

        break;
      }
      case kOpReturn: {
        StackValue stack_value = {};

        // cppcheck-suppress redundantInitialization
        stack_value = Pop();

        PopCallFrame(&stack_value);

        break;
      }
      case kOpPushCallFrame: {
        instruction_address += 2;

        break;
      }
      case kOpPopCallFrame: {
        break;
      }
      case kOpMakeArray: {
        const size_t kElementCount = instructions[instruction_address++];
        StackValue val;
        size_t element = 0;
        Array* array_instance = &array_pool[array_pool_index++];
        StackValue array_value = {0};
        array_value.type = kVariableTypeArray;
        array_value.as.array = array_instance;

        if (kElementCount > kArrayElementsMax) {
          puts("Error: Array too large.");
          return;
        }

        if (array_pool_index >= kArrayPoolSize) {
          puts("Error: Too many arrays.");
          return;
        }

        for (element = 0; element < kElementCount; ++element) {
          val = Pop();
          if (val.type != kVariableTypeInt) {
            puts("Error: Only integer are supported in arrays.");
            return;
          }
          array_instance->elements[kElementCount - element - 1] = val.as.number;
        }
        array_instance->count = kElementCount;

        Push(array_value);
        break;
      }
      case kOpIndexArray: {
        StackValue index;
        StackValue array;
        StackValue result;

        index = Pop();
        array = Pop();
        if (array.type != kVariableTypeArray) {
          puts("Runtime error: Cannot index non-array.");
          return;
        }

        if (index.type != kVariableTypeInt) {
          puts("Runtime error: Array index must be integer.");
          return;
        }

        if (index.as.number < 0 ||
            (size_t)index.as.number >= array.as.array->count) {
          puts("Runtime error: Array index out of bounds.");
          return;
        }
        result.type = kVariableTypeInt;
        result.as.number = array.as.array->elements[index.as.number];

        Push(result);
        break;
      }
      case kOpStoreElement: {
        const size_t kArrayIndex = instructions[instruction_address++];
        StackValue value;
        StackValue index;
        StackValue array;

        value = Pop();
        index = Pop();
        array = global_variables[kArrayIndex];

        if (array.type != kVariableTypeArray || !array.as.array) {
          puts("Runtime error: This is not an array.");
          return;
        }

        if (index.type != kVariableTypeInt) {
          puts("Runtime error: Array index must be integer.");
          return;
        }

        if (value.type != kVariableTypeInt) {
          puts("Runtime error: Only integers can be stored in arrays.");
          return;
        }

        if (index.as.number < 0 ||
            (size_t)index.as.number >= array.as.array->count) {
          puts("Runtime error: Array index out of bounds.");
          return;
        }

        array.as.array->elements[index.as.number] = value.as.number;
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