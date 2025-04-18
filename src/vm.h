#ifndef VM_H
#define VM_H

#if defined(__CC65__) || defined(__linux__)
#include <stddef.h>
#elif __APPLE__
#include <sys/_types/_size_t.h>
#endif

#ifdef __CC65__
enum { kInstructionsSize = 128 };
#else
static constexpr int kInstructionsSize = 128;
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

// NOLINTBEGIN(cppcoreguidelines-avoid-non-const-global-variables)
extern unsigned char instructions[];
extern size_t instruction_index;
// NOLINTEND(cppcoreguidelines-avoid-non-const-global-variables)

void ResetInterpreterState();

void EmitByte(unsigned char byte);

void EmitHalt();

void RemoveHalt();

size_t AddNumberConstant(int value, ConstantType constant_type);

size_t AddStringConstant(const char* string);

void RunVm();

void PrintOpcodes();

#endif  // VM_H
