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
  TEST_ASSERT_EQUAL_STRING("print", token.value.text);

  source_code = "print(  3 +2   )";

  ConsumeNextToken();
  TEST_ASSERT_EQUAL_INT(kTokenPrint, token.type);
  TEST_ASSERT_EQUAL_STRING("print", token.value.text);

  ConsumeNextToken();
  ConsumeNextToken();
  TEST_ASSERT_EQUAL_INT(kTokenNumber, token.type);
  TEST_ASSERT_EQUAL_INT(3, token.value.number);

  ConsumeNextToken();
  ConsumeNextToken();
  ConsumeNextToken();
  TEST_ASSERT_EQUAL_INT(kTokenRightParenthesis, token.type);
  TEST_ASSERT_EQUAL_STRING(")", token.value.text);
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

static void TestGreaterThan() {
  source_code = ">flkd";

  ConsumeNextToken();
  TEST_ASSERT_EQUAL_INT(kTokenGreaterThan, token.type);
  TEST_ASSERT_EQUAL_STRING(">", token.value.text);
}

static void TestLessThan() {
  source_code = "<jvmv";

  ConsumeNextToken();
  TEST_ASSERT_EQUAL_INT(kTokenLessThan, token.type);
  TEST_ASSERT_EQUAL_STRING("<", token.value.text);
}

static void TestGreaterThanOrEqualTo() {
  source_code = ">=fjkle";

  ConsumeNextToken();
  TEST_ASSERT_EQUAL_INT(kTokenGreaterOrEquals, token.type);
  TEST_ASSERT_EQUAL_STRING(">=", token.value.text);
}

static void TestLessThanOrEqualTo() {
  source_code = "<=radf";

  ConsumeNextToken();
  TEST_ASSERT_EQUAL_INT(kTokenLessOrEquals, token.type);
  TEST_ASSERT_EQUAL_STRING("<=", token.value.text);
}

static void TestBooleanLiteral() {
  source_code = "true";

  ConsumeNextToken();
  TEST_ASSERT_EQUAL_INT(kTokenBoolean, token.type);
  TEST_ASSERT_EQUAL_INT(1, token.value.number);

  source_code = "false";

  ConsumeNextToken();
  TEST_ASSERT_EQUAL_INT(kTokenBoolean, token.type);
  TEST_ASSERT_EQUAL_INT(0, token.value.number);

  source_code = "x: bool=false";

  ConsumeNextToken();
  ConsumeNextToken();
  ConsumeNextToken();
  ConsumeNextToken();
  ConsumeNextToken();
  TEST_ASSERT_EQUAL_INT(kTokenBoolean, token.type);
  TEST_ASSERT_EQUAL_INT(0, token.value.number);
}

int main() {
  UNITY_BEGIN();

  // RUN_TEST(TestInvalidToken);
  RUN_TEST(TestSkipWhitespace);
  RUN_TEST(TestPrintArithmetic);
  RUN_TEST(TestString);
  RUN_TEST(TestGreaterThan);
  RUN_TEST(TestLessThan);
  RUN_TEST(TestGreaterThanOrEqualTo);
  RUN_TEST(TestLessThanOrEqualTo);
  RUN_TEST(TestBooleanLiteral);

  return UNITY_END();
}
