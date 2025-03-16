#include "lexer.h"

#include <unity.h>
#include <unity_internals.h>

void setUp() {}

void tearDown() {}

// TODO(Martin): Fix lexer so this test passes
static void TestInvalidToken() {
  source_code = "jj#@%!$";

  ConsumeNextToken();
  TEST_ASSERT_EQUAL(kTokenEof, token.type);
}

static void TestSkipWhitespace() {
  source_code = "     print(3+2)";

  ConsumeNextToken();
  TEST_ASSERT_EQUAL_INT(kTokenPrint, token.type);
}

static void TestPrintArithmetic() {
  source_code = "print(3+2)";

  ConsumeNextToken();
  TEST_ASSERT_EQUAL_INT(kTokenPrint, token.type);
  TEST_ASSERT_EQUAL_STRING("print", token.value.text);

  ConsumeNextToken();
  TEST_ASSERT_EQUAL_INT(kTokenLeftParenthesis, token.type);
  TEST_ASSERT_EQUAL_STRING("(", token.value.text);

  ConsumeNextToken();
  TEST_ASSERT_EQUAL_INT(kTokenNumber, token.type);
  TEST_ASSERT_EQUAL_INT(3, token.value.number);

  ConsumeNextToken();
  TEST_ASSERT_EQUAL_INT(kTokenPlus, token.type);
  TEST_ASSERT_EQUAL_STRING("+", token.value.text);

  ConsumeNextToken();
  TEST_ASSERT_EQUAL_INT(kTokenNumber, token.type);
  TEST_ASSERT_EQUAL_INT(2, token.value.number);

  ConsumeNextToken();
  TEST_ASSERT_EQUAL_INT(kTokenRightParenthesis, token.type);
  TEST_ASSERT_EQUAL_STRING(")", token.value.text);

  ConsumeNextToken();
  TEST_ASSERT_EQUAL_INT(kTokenEof, token.type);
}

// TODO(Martin): Fix lexer so strings have their own token type
static void TestString() {
  source_code = "\"Hello, world!\"";

  ConsumeNextToken();
  TEST_ASSERT_EQUAL_INT(kTokenQuotationMark, token.type);
  TEST_ASSERT_EQUAL_STRING("Hello, world!", token.value.text);
}

int main() {
  UNITY_BEGIN();

  // RUN_TEST(TestInvalidToken);
  RUN_TEST(TestSkipWhitespace);
  RUN_TEST(TestPrintArithmetic);
  RUN_TEST(TestString);

  return UNITY_END();
}
