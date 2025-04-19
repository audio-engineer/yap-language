#ifndef LEXER_TEST_H
#define LEXER_TEST_H

void TestUndefinedToken();
void TestSkipWhitespace();
void TestPrintArithmetic();
void TestMinus();
void TestStar();
void TestSlash();
void TestString();
void TestPrintString();
void TestIdentifier();
void TestGreaterThan();
void TestLessThan();
void TestGreaterThanOrEqualTo();
void TestLessThanOrEqualTo();
void TestBooleanLiteral();
void TestIf();
void TestFor();
void TestNot();
void TestIntVariableDeclaration();
void TestBoolVariableDeclaration();
void TestStrVariableDeclaration();
void TestFloatVariableDeclaration();
void TestVariableAssignment();
void TestPrintIdentifier();
void TestJumpOverNewline();
void TestFunctionDeclaration();
void TestFunctionDeclarationPrintAndCall();
void TestFunctionCallComma();
void TestFunctionCallInPrint();

#endif  // LEXER_TEST_H
