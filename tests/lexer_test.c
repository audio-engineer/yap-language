#include "lexer.h"

#include <string.h>
#include <unity.h>

#include "lexer_test.h"

void SetTest(const char* string_input) {
  const size_t kStringLength = strlen(string_input);
  // NOLINTNEXTLINE(clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling,-warnings-as-errors)
  memcpy(program_buffer, string_input, kStringLength);
  program_buffer[kStringLength] = '\0';
  program_buffer_index = 0;
}

void TestUndefinedToken() {
  SetTest("^");

  ConsumeNextToken();  // ^
  TEST_ASSERT_EQUAL_INT(kTokenEof, token.type);
}

void TestSkipWhitespace() {
  SetTest("     print(3+2)");

  ConsumeNextToken();  // print
  TEST_ASSERT_EQUAL_INT(kTokenPrint, token.type);

  SetTest("print(  3 +2   )");

  ConsumeNextToken();  // print
  TEST_ASSERT_EQUAL_INT(kTokenPrint, token.type);

  ConsumeNextToken();  // (
  ConsumeNextToken();  // 3
  TEST_ASSERT_EQUAL_INT(kTokenNumber, token.type);
  TEST_ASSERT_EQUAL_INT(3, token.value.number);

  ConsumeNextToken();  // +
  ConsumeNextToken();  // 2
  ConsumeNextToken();  // )
  TEST_ASSERT_EQUAL_INT(kTokenRightParenthesis, token.type);
}

void TestPrintArithmetic() {
  SetTest("print(3+2)");

  ConsumeNextToken();  // print
  TEST_ASSERT_EQUAL_INT(kTokenPrint, token.type);

  ConsumeNextToken();  // (
  TEST_ASSERT_EQUAL_INT(kTokenLeftParenthesis, token.type);

  ConsumeNextToken();  // 3
  TEST_ASSERT_EQUAL_INT(kTokenNumber, token.type);
  TEST_ASSERT_EQUAL_INT(3, token.value.number);

  ConsumeNextToken();  // +
  TEST_ASSERT_EQUAL_INT(kTokenPlus, token.type);

  ConsumeNextToken();  // 2
  TEST_ASSERT_EQUAL_INT(kTokenNumber, token.type);
  TEST_ASSERT_EQUAL_INT(2, token.value.number);

  ConsumeNextToken();  // )
  TEST_ASSERT_EQUAL_INT(kTokenRightParenthesis, token.type);

  ConsumeNextToken();  // EOF
  TEST_ASSERT_EQUAL_INT(kTokenEof, token.type);
}

void TestMinus() {
  SetTest("20-15");

  ConsumeNextToken();  // 20
  ConsumeNextToken();  // -
  TEST_ASSERT_EQUAL_INT(kTokenMinus, token.type);
}

void TestStar() {
  SetTest("4*97");

  ConsumeNextToken();  // 4
  ConsumeNextToken();  // *
  TEST_ASSERT_EQUAL_INT(kTokenStar, token.type);
}

void TestSlash() {
  SetTest("60/3");

  ConsumeNextToken();  // 60
  ConsumeNextToken();  // /
  TEST_ASSERT_EQUAL_INT(kTokenSlash, token.type);
}

void TestString() {
  SetTest("\"Hello, world!\"");

  ConsumeNextToken();  // Hello, world!
  TEST_ASSERT_EQUAL_INT(kTokenString, token.type);
  TEST_ASSERT_EQUAL_STRING("Hello, world!", token.value.text);
}

void TestPrintString() {
  SetTest("print(\"Hello, world!\")");

  ConsumeNextToken();  // print
  ConsumeNextToken();  // (
  ConsumeNextToken();  // Hello, world!
  TEST_ASSERT_EQUAL_INT(kTokenString, token.type);
  TEST_ASSERT_EQUAL_STRING("Hello, world!", token.value.text);
}

void TestIdentifier() {
  SetTest("variable:int=");

  ConsumeNextToken();  // variable
  TEST_ASSERT_EQUAL_INT(kTokenId, token.type);
  TEST_ASSERT_EQUAL_STRING("variable", token.value.text);
}

void TestGreaterThan() {
  SetTest(">flkd");

  ConsumeNextToken();  // >
  TEST_ASSERT_EQUAL_INT(kTokenGreaterThan, token.type);
}

void TestLessThan() {
  SetTest("<feflkd");

  ConsumeNextToken();  // <
  TEST_ASSERT_EQUAL_INT(kTokenLessThan, token.type);
}

void TestGreaterThanOrEqualTo() {
  SetTest(">=jffevmv");

  ConsumeNextToken();  // >=
  TEST_ASSERT_EQUAL_INT(kTokenGreaterOrEquals, token.type);
}

void TestLessThanOrEqualTo() {
  SetTest("<=grgrjvmv");

  ConsumeNextToken();  // <=
  TEST_ASSERT_EQUAL_INT(kTokenLessOrEquals, token.type);
}

void TestBooleanLiteral() {
  SetTest("true");

  ConsumeNextToken();  // true
  TEST_ASSERT_EQUAL_INT(kTokenBoolean, token.type);
  TEST_ASSERT_EQUAL_INT(1, token.value.number);

  SetTest("false");

  ConsumeNextToken();  // false
  TEST_ASSERT_EQUAL_INT(kTokenBoolean, token.type);
  TEST_ASSERT_EQUAL_INT(0, token.value.number);

  SetTest("x: bool=false");

  ConsumeNextToken();  // x
  ConsumeNextToken();  // :
  ConsumeNextToken();  // bool
  ConsumeNextToken();  // =
  ConsumeNextToken();  // false
  TEST_ASSERT_EQUAL_INT(kTokenBoolean, token.type);
  TEST_ASSERT_EQUAL_INT(0, token.value.number);
}

void TestIf() {
  SetTest("if(true)print(\"Hello, world!\")endif");

  ConsumeNextToken();  // if
  TEST_ASSERT_EQUAL_INT(kTokenIf, token.type);

  ConsumeNextToken();  // (
  ConsumeNextToken();  // true
  ConsumeNextToken();  // )
  ConsumeNextToken();  // print
  ConsumeNextToken();  // (
  ConsumeNextToken();  // Hello, world!
  ConsumeNextToken();  // )
  ConsumeNextToken();  // endif
  TEST_ASSERT_EQUAL_INT(kTokenEndif, token.type);
}

void TestFor() {
  SetTest("for(true)print(\"Hello, world!\")endfor");

  ConsumeNextToken();  // for
  TEST_ASSERT_EQUAL_INT(kTokenFor, token.type);

  ConsumeNextToken();  // (
  ConsumeNextToken();  // true
  ConsumeNextToken();  // )
  ConsumeNextToken();  // print
  ConsumeNextToken();  // (
  ConsumeNextToken();  // Hello, world!
  ConsumeNextToken();  // )
  ConsumeNextToken();  // endif
  TEST_ASSERT_EQUAL_INT(kTokenEndfor, token.type);
}

void TestNot() {
  SetTest("print(!true)");

  ConsumeNextToken();  // print
  ConsumeNextToken();  // (
  ConsumeNextToken();  // !
  TEST_ASSERT_EQUAL_INT(kTokenNot, token.type);

  ConsumeNextToken();  // true
  TEST_ASSERT_EQUAL_INT(kTokenBoolean, token.type);
}
