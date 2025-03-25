#ifndef VM_H
#define VM_H

#ifdef __CC65__
#include <stdbool.h>
#endif

#if defined(__CC65__) || defined(__linux__)
#include <stddef.h>
#elif __APPLE__
#include <sys/_types/_size_t.h>
#endif

typedef enum Opcode {
  kOpConstant,
  kOpAdd,
  kOpSubtract,
  kOpMultiply,
  kOpDivide,
  kOpEquals,
  kOpNotEquals,
  kOpGreaterThan,
  kOpGreaterOrEquals,
  kOpLessThan,
  kOpLessOrEquals,
  kOpPrint,
  kOpJumpIfFalse,
  kOpJump,
  kOpHalt
} Opcode;

typedef enum ConstantType {
  kTypeNumber,
  kTypeString,
  kTypeBoolean
} ConstantType;

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

typedef struct Constants {
  const void* pointer[kConstantsSize];
  ConstantType type[kConstantsSize];
} Constants;

// NOLINTBEGIN(cppcoreguidelines-avoid-non-const-global-variables)
extern unsigned char opcodes[];
extern size_t opcode_index;

extern Constants constants;
extern size_t constants_index;

extern char string_pool[];
extern size_t string_pool_index;

extern long number_pool[];
extern size_t number_pool_index;

extern size_t stack[];
extern size_t stack_index;
// NOLINTEND(cppcoreguidelines-avoid-non-const-global-variables)

void ResetInterpreterState();

void EmitByte(unsigned char byte);

size_t AddNumberConstant(long value, ConstantType constant_type);

size_t AddStringConstant(const char* string);

void RunVm();

void PrintOpcodes();

#endif  // VM_H
