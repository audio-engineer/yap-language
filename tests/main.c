#include <unity.h>
#include <unity_internals.h>

#include "conditionals_test.h"
#include "lexer_test.h"
#include "parser_test.h"

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
  RUN_TEST(TestNot);

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

  // Conditionals
  RUN_TEST(TestIfTrueExecutesBlock);
  RUN_TEST(TestIfFalseSkipsBlock);
  RUN_TEST(TestIfGreaterThanComparison);
  RUN_TEST(TestIfLessThanComparison);
  RUN_TEST(TestElse);

  return UNITY_END();
}
