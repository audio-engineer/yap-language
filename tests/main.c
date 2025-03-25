#include <unity.h>
#include <unity_internals.h>

// lexer
extern void TestUndefinedToken();
extern void TestSkipWhitespace();
extern void TestPrintArithmetic();
extern void TestMinus();
extern void TestStar();
extern void TestSlash();
extern void TestString();
extern void TestPrintString();
extern void TestIdentifier();
extern void TestGreaterThan();
extern void TestLessThan();
extern void TestGreaterThanOrEqualTo();
extern void TestLessThanOrEqualTo();
extern void TestBooleanLiteral();
extern void TestIf();
extern void TestFor();

// parser
extern void TestRecursiveArithmetic();
extern void TestLessThanCondition();
extern void TestLessOrEqualCondition();
extern void TestGreaterThanCondition();
extern void TestGreaterOrEqualCondition();
extern void TestEqualCondition();
extern void TestNotEqualCondition();
extern void TestAddArithmetic();
extern void TestSubtractArithmetic();
extern void TestMultiplyArithmetic();
extern void TestDivideArithmetic();
extern void TestTrueBoolean();
extern void TestFalseBoolean();
extern void TestStringParse();
extern void TestUnregisteredStatement();
extern void TestNoExpression();
extern void TestMissingLeftParen();

extern void TestIfTrueExecutesBlock();
extern void TestIfFalseSkipsBlock();
extern void TestIfGreaterThanComparison();
extern void TestIfLessThanComparison();

void setUp() {}

void tearDown() {}

int main() {
  UNITY_BEGIN();

  // Lexer
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

  // Parser
  RUN_TEST(TestRecursiveArithmetic);
  RUN_TEST(TestLessThanCondition);
  RUN_TEST(TestLessOrEqualCondition);
  RUN_TEST(TestGreaterThanCondition);
  RUN_TEST(TestGreaterOrEqualCondition);
  RUN_TEST(TestEqualCondition);
  RUN_TEST(TestNotEqualCondition);
  RUN_TEST(TestAddArithmetic);
  RUN_TEST(TestSubtractArithmetic);
  RUN_TEST(TestMultiplyArithmetic);
  RUN_TEST(TestDivideArithmetic);
  RUN_TEST(TestTrueBoolean);
  RUN_TEST(TestFalseBoolean);
  RUN_TEST(TestStringParse);
  RUN_TEST(TestUnregisteredStatement);
  RUN_TEST(TestNoExpression);
  RUN_TEST(TestMissingLeftParen);

  // conditionals
  RUN_TEST(TestIfTrueExecutesBlock);
  RUN_TEST(TestIfFalseSkipsBlock);
  RUN_TEST(TestIfGreaterThanComparison);
  RUN_TEST(TestIfLessThanComparison);

  return UNITY_END();
}
