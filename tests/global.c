#include "global.h"

#include <lexer.h>
#include <parser.h>
#include <string.h>
#include <vm.h>

void FillProgramBuffer(const char* const program) {
  ResetLexerState();
  ResetParserState();
  ResetInterpreterState();

  const size_t kStringLength = strlen(program);

  // NOLINTNEXTLINE(clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling)
  memcpy(program_buffer, program, kStringLength);

  program_buffer[kStringLength] = '\0';
  program_buffer_index = 0;
}

void FillProgramBufferAndParse(const char* const program) {
  FillProgramBuffer(program);
  ParseProgram();
  EmitByte(kOpHalt);

  constants_index = 0;
}
