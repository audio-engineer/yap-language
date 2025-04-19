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
  puts("Lexer");
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
  RUN_TEST(TestIntVariableDeclaration);
  RUN_TEST(TestBoolVariableDeclaration);
  RUN_TEST(TestStrVariableDeclaration);
  RUN_TEST(TestFloatVariableDeclaration);
  RUN_TEST(TestVariableAssignment);
  RUN_TEST(TestPrintIdentifier);
  RUN_TEST(TestJumpOverNewline);
  RUN_TEST(TestFunctionDeclaration);
  RUN_TEST(TestFunctionDeclarationPrintAndCall);
  RUN_TEST(TestFunctionCallComma);
  RUN_TEST(TestFunctionCallInPrint);

  // Parser
  puts("");
  puts("Parser");
  RUN_TEST(TestRecursiveArithmetic);
  RUN_TEST(TestParenthesesArithmetic);
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
  RUN_TEST(TestDeclareIntAndPrint);
  RUN_TEST(TestDeclareBool);
  RUN_TEST(TestDeclareStr);
  RUN_TEST(TestDeclareFloat);
  RUN_TEST(TestDeclareIntAssignAndPrint);
  RUN_TEST(TestDeclareFunctionOneParameter);
  RUN_TEST(TestDeclareFunctionTwoParameters);
  RUN_TEST(TestDeclareAndCallFunction);
  RUN_TEST(TestUnregisteredStatement);
  RUN_TEST(TestMissingLeftParen);

  // Conditionals
  RUN_TEST(TestIfTrueExecutesBlock);
  RUN_TEST(TestIfFalseSkipsBlock);
  RUN_TEST(TestIfGreaterThanComparison);
  RUN_TEST(TestIfLessThanComparison);
  RUN_TEST(TestElse);

  return UNITY_END();
}
