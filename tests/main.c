#include <stdio.h>
#include <unity.h>
#include <unity_internals.h>

#include "lexer_test.h"
#include "parser_test.h"
#include "vm_test.h"

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
  RUN_TEST(TestPercent);
  RUN_TEST(TestString);
  RUN_TEST(TestPrintString);
  RUN_TEST(TestIdentifier);
  RUN_TEST(TestGreaterThan);
  RUN_TEST(TestLessThan);
  RUN_TEST(TestGreaterThanOrEqualTo);
  RUN_TEST(TestLessThanOrEqualTo);
  RUN_TEST(TestBooleanLiteral);
  RUN_TEST(TestIf);
  RUN_TEST(TestNestedIf);
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
  RUN_TEST(TestModuloArithmetic);
  RUN_TEST(TestTrueBoolean);
  RUN_TEST(TestFalseBoolean);
  RUN_TEST(TestStringParse);
  RUN_TEST(TestDeclareIntAndPrint);
  RUN_TEST(TestDeclareBool);
  RUN_TEST(TestDeclareStr);
  // TODO(Martin): Enable when floats work.
  // RUN_TEST(TestDeclareFloat);
  RUN_TEST(TestDeclareIntAssignAndPrint);
  // TODO(Martin): Enable when functions work without parameters.
  // RUN_TEST(TestDeclareFunctionWithNoParameters);
  RUN_TEST(TestDeclareFunctionOneParameter);
  RUN_TEST(TestDeclareFunctionTwoParameters);
  RUN_TEST(TestDeclareAndCallFunction);
  RUN_TEST(TestUnregisteredStatement);
  RUN_TEST(TestMissingLeftParen);

  // vm tests
  puts("");
  puts("VM");
  RUN_TEST(TestPlusOperator);
  RUN_TEST(TestMinusOperator);
  RUN_TEST(TestMultiplyOperator);
  RUN_TEST(TestDivideOperator);
  RUN_TEST(TestModuloOperator);
  RUN_TEST(TestEqualOperator);
  RUN_TEST(TestNotEqualOperator);
  RUN_TEST(TestGreaterThanOperator);
  RUN_TEST(TestGreaterOrEqualsOperator);
  RUN_TEST(TestLessThanOperator);
  RUN_TEST(TestLessOrEqualsOperator);
  // UNIMPLEMENTED
  // RUN_TEST(TestOrOperator);
  // RUN_TEST(TestAndOperator);
  RUN_TEST(TestIfTrueExecutesBlock);
  RUN_TEST(TestIfFalseSkipsBlock);
  RUN_TEST(TestIfGreaterThanComparison);
  RUN_TEST(TestIfLessThanComparison);
  RUN_TEST(TestElse);
  RUN_TEST(TestNestedConditionals);

  // Acceptance testing
  puts("");
  puts("Acceptance testing");
  RUN_TEST(Example1);
  RUN_TEST(Example2);
  RUN_TEST(Example3);
  RUN_TEST(Example4);
  // RUN_TEST(Example5);
  RUN_TEST(Example6);
  // RUN_TEST(Example7);
  // RUN_TEST(Example8);
  // RUN_TEST(Example9);
  // RUN_TEST(Example10);

  // Loops
  // RUN_TEST(TestForLoopExecutesThreeTimes);
  RUN_TEST(TestWhileLoopExecutesThreeTimes);
  RUN_TEST(TestNestedWhileLoops);
  return UNITY_END();
}
