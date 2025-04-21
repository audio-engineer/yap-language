#ifndef PARSER_TEST_H
#define PARSER_TEST_H

void TestRecursiveArithmetic();
void TestParenthesesArithmetic();
void TestLessThanCondition();
void TestLessOrEqualCondition();
void TestGreaterThanCondition();
void TestGreaterOrEqualCondition();
void TestEqualCondition();
void TestNotEqualCondition();
void TestAddArithmetic();
void TestSubtractArithmetic();
void TestMultiplyArithmetic();
void TestDivideArithmetic();
void TestTrueBoolean();
void TestFalseBoolean();
void TestStringParse();
void TestDeclareIntAndPrint();
void TestDeclareBool();
void TestDeclareStr();
void TestDeclareFloat();
void TestDeclareIntAssignAndPrint();
// TODO(Martin): Enable when functions work without parameters.
// void TestDeclareFunctionWithNoParameters();
void TestDeclareFunctionOneParameter();
void TestDeclareFunctionTwoParameters();
void TestDeclareAndCallFunction();
void TestUnregisteredStatement();
void TestMissingLeftParen();

#endif  // PARSER_TEST_H
