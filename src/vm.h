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
  kOpGreaterThan,
  kOpGreaterOrEquals,
  kOpLessThan,
  kOpLessOrEquals,
  kOpPrint,
  kOpIf,
  kOpHalt
} Opcode;

typedef enum ConstantType {
  kTypeNumber,
  kTypeString,
  kTypeBoolean
} ConstantType;

// NOLINTBEGIN(cppcoreguidelines-avoid-non-const-global-variables)
extern unsigned char opcodes[];
extern size_t opcode_index;
// NOLINTEND(cppcoreguidelines-avoid-non-const-global-variables)

void EmitByte(unsigned char byte);

size_t AddNumberConstant(long value, ConstantType constant_type);

size_t AddStringConstant(const char* string);

void RunVm();

void PrintOpcodes();

#endif  // VM_H
