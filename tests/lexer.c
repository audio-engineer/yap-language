#include "lexer.h"

#include <unity.h>
#include <unity_internals.h>

void setUp() {}

void tearDown() {}

static void TestUndefinedToken() {
  source_code = "^";

  ConsumeNextToken();  // ^
  TEST_ASSERT_EQUAL_INT(kTokenEof, token.type);
}

static void TestSkipWhitespace() {
  source_code = "     print(3+2)";

  ConsumeNextToken();  // print
  TEST_ASSERT_EQUAL_INT(kTokenPrint, token.type);

  source_code = "print(  3 +2   )";

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

static void TestPrintArithmetic() {
  source_code = "print(3+2)";

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

static void TestMinus() {
  source_code = "20-15";

  ConsumeNextToken();  // 20
  ConsumeNextToken();  // -
  TEST_ASSERT_EQUAL_INT(kTokenMinus, token.type);
}

static void TestStar() {
  source_code = "4*97";

  ConsumeNextToken();  // 4
  ConsumeNextToken();  // *
  TEST_ASSERT_EQUAL_INT(kTokenStar, token.type);
}

static void TestSlash() {
  source_code = "60/3";

  ConsumeNextToken();  // 60
  ConsumeNextToken();  // /
  TEST_ASSERT_EQUAL_INT(kTokenSlash, token.type);
}

static void TestString() {
  source_code = "\"Hello, world!\"";

  ConsumeNextToken();  // Hello, world!
  TEST_ASSERT_EQUAL_INT(kTokenString, token.type);
  TEST_ASSERT_EQUAL_STRING("Hello, world!", token.value.text);
}

static void TestPrintString() {
  source_code = "print(\"Hello, world!\")";

  ConsumeNextToken();  // print
  ConsumeNextToken();  // (
  ConsumeNextToken();  // Hello, world!
  TEST_ASSERT_EQUAL_INT(kTokenString, token.type);
  TEST_ASSERT_EQUAL_STRING("Hello, world!", token.value.text);
}

static void TestIdentifier() {
  source_code = "variable:int=";

  ConsumeNextToken();  // variable
  TEST_ASSERT_EQUAL_INT(kTokenId, token.type);
  TEST_ASSERT_EQUAL_STRING("variable", token.value.text);
}

static void TestGreaterThan() {
  source_code = ">flkd";

  ConsumeNextToken();  // >
  TEST_ASSERT_EQUAL_INT(kTokenGreaterThan, token.type);
}

static void TestLessThan() {
  source_code = "<jvmv";

  ConsumeNextToken();  // <
  TEST_ASSERT_EQUAL_INT(kTokenLessThan, token.type);
}

static void TestGreaterThanOrEqualTo() {
  source_code = ">=fjkle";

  ConsumeNextToken();  // >=
  TEST_ASSERT_EQUAL_INT(kTokenGreaterOrEquals, token.type);
}

static void TestLessThanOrEqualTo() {
  source_code = "<=radf";

  ConsumeNextToken();  // <=
  TEST_ASSERT_EQUAL_INT(kTokenLessOrEquals, token.type);
}

static void TestBooleanLiteral() {
  source_code = "true";

  ConsumeNextToken();  // true
  TEST_ASSERT_EQUAL_INT(kTokenBoolean, token.type);
  TEST_ASSERT_EQUAL_INT(1, token.value.number);

  source_code = "false";

  ConsumeNextToken();  // false
  TEST_ASSERT_EQUAL_INT(kTokenBoolean, token.type);
  TEST_ASSERT_EQUAL_INT(0, token.value.number);

  source_code = "x: bool=false";

  ConsumeNextToken();  // x
  ConsumeNextToken();  // :
  ConsumeNextToken();  // bool
  ConsumeNextToken();  // =
  ConsumeNextToken();  // false
  TEST_ASSERT_EQUAL_INT(kTokenBoolean, token.type);
  TEST_ASSERT_EQUAL_INT(0, token.value.number);
}

static void TestIf() {
  source_code = "if(true)print(\"Hello, world!\")endif";

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

static void TestFor() {
  source_code = "for(true)print(\"Hello, world!\")endfor";

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

int main() {
  UNITY_BEGIN();

  RUN_TEST(TestUndefinedToken);
  RUN_TEST(TestSkipWhitespace);
  RUN_TEST(TestPrintArithmetic);
  RUN_TEST(TestMinus);
  RUN_TEST(TestStar);
  RUN_TEST(TestSlash);
  RUN_TEST(TestString);
  RUN_TEST(TestPrintString);
  RUN_TEST(TestIdentifier);
  RUN_TEST(TestGreaterThan);
  RUN_TEST(TestLessThan);
  RUN_TEST(TestGreaterThanOrEqualTo);
  RUN_TEST(TestLessThanOrEqualTo);
  RUN_TEST(TestBooleanLiteral);
  RUN_TEST(TestIf);
  RUN_TEST(TestFor);

  return UNITY_END();
}
