#ifndef VM_H
#define VM_H

#ifdef COMMODORE
#include <stddef.h>
#elif __linux__
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
  kOpPrint,
  kOpIf,
  kOpHalt
} Opcode;

// NOLINTBEGIN(cppcoreguidelines-avoid-non-const-global-variables)
extern unsigned char opcodes[];
extern size_t opcode_index;
// NOLINTEND(cppcoreguidelines-avoid-non-const-global-variables)

void EmitByte(unsigned char byte);

size_t AddNumberConstant(long number);

size_t AddStringConstant(const char* string);

void RunVm();

void PrintOpcodes();

#endif  // VM_H
