#include "lexer_test.h"

#include <lexer.h>
#include <unity.h>
#include <vm.h>

#include "global.h"

void TestUndefinedToken() {
  FillProgramBuffer("^");

  ConsumeNextToken();  // ^
  TEST_ASSERT_EQUAL_INT(kTokenEof, token.type);
  TEST_ASSERT_EQUAL_INT(0, program_buffer_index);
}

void TestSkipWhitespace() {
  FillProgramBuffer("     print(3+2)");

  ConsumeNextToken();  // print
  TEST_ASSERT_EQUAL_INT(kTokenPrint, token.type);
  TEST_ASSERT_EQUAL_INT(10, program_buffer_index);

  ResetInterpreterState();
  program_buffer_index = 0;

  FillProgramBuffer("print(  3 +2   )");

  ConsumeNextToken();  // print
  TEST_ASSERT_EQUAL_INT(kTokenPrint, token.type);
  TEST_ASSERT_EQUAL_INT(5, program_buffer_index);

  ConsumeNextToken();  // (
  ConsumeNextToken();  // 3
  TEST_ASSERT_EQUAL_INT(kTokenNumber, token.type);
  TEST_ASSERT_EQUAL_INT(3, token.value.number);
  TEST_ASSERT_EQUAL_INT(9, program_buffer_index);

  ConsumeNextToken();  // +
  ConsumeNextToken();  // 2
  ConsumeNextToken();  // )
  TEST_ASSERT_EQUAL_INT(kTokenRightParenthesis, token.type);
  TEST_ASSERT_EQUAL_INT(16, program_buffer_index);
}

void TestPrintArithmetic() {
  FillProgramBuffer("print(3+2)");

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
  FillProgramBuffer("20-15");

  ConsumeNextToken();  // 20
  ConsumeNextToken();  // -
  TEST_ASSERT_EQUAL_INT(kTokenMinus, token.type);
}

void TestStar() {
  FillProgramBuffer("4*97");

  ConsumeNextToken();  // 4
  ConsumeNextToken();  // *
  TEST_ASSERT_EQUAL_INT(kTokenStar, token.type);
}

void TestSlash() {
  FillProgramBuffer("60/3");

  ConsumeNextToken();  // 60
  ConsumeNextToken();  // /
  TEST_ASSERT_EQUAL_INT(kTokenSlash, token.type);
}

void TestString() {
  FillProgramBuffer("\"Hello, world!\"");

  ConsumeNextToken();  // Hello, world!
  TEST_ASSERT_EQUAL_INT(kTokenString, token.type);
  TEST_ASSERT_EQUAL_STRING("Hello, world!", token.value.text);
}

void TestPrintString() {
  FillProgramBuffer("print(\"Hello, world!\")");

  ConsumeNextToken();  // print
  ConsumeNextToken();  // (
  ConsumeNextToken();  // Hello, world!
  TEST_ASSERT_EQUAL_INT(kTokenString, token.type);
  TEST_ASSERT_EQUAL_STRING("Hello, world!", token.value.text);
}

void TestIdentifier() {
  FillProgramBuffer("variable:int=");

  ConsumeNextToken();  // variable
  TEST_ASSERT_EQUAL_INT(kTokenIdentifier, token.type);
  TEST_ASSERT_EQUAL_STRING("variable", token.value.text);
}

void TestGreaterThan() {
  FillProgramBuffer(">flkd");

  ConsumeNextToken();  // >
  TEST_ASSERT_EQUAL_INT(kTokenGreaterThan, token.type);
}

void TestLessThan() {
  FillProgramBuffer("<feflkd");

  ConsumeNextToken();  // <
  TEST_ASSERT_EQUAL_INT(kTokenLessThan, token.type);
}

void TestGreaterThanOrEqualTo() {
  FillProgramBuffer(">=jffevmv");

  ConsumeNextToken();  // >=
  TEST_ASSERT_EQUAL_INT(kTokenGreaterOrEquals, token.type);
}

void TestLessThanOrEqualTo() {
  FillProgramBuffer("<=grgrjvmv");

  ConsumeNextToken();  // <=
  TEST_ASSERT_EQUAL_INT(kTokenLessOrEquals, token.type);
}

void TestBooleanLiteral() {
  FillProgramBuffer("true");

  ConsumeNextToken();  // true
  TEST_ASSERT_EQUAL_INT(kTokenBoolean, token.type);
  TEST_ASSERT_EQUAL_INT(1, token.value.number);

  FillProgramBuffer("false");

  ConsumeNextToken();  // false
  TEST_ASSERT_EQUAL_INT(kTokenBoolean, token.type);
  TEST_ASSERT_EQUAL_INT(0, token.value.number);

  FillProgramBuffer("x: bool=false");

  ConsumeNextToken();  // x
  ConsumeNextToken();  // :
  ConsumeNextToken();  // bool
  ConsumeNextToken();  // =
  ConsumeNextToken();  // false
  TEST_ASSERT_EQUAL_INT(kTokenBoolean, token.type);
  TEST_ASSERT_EQUAL_INT(0, token.value.number);
}

void TestIf() {
  FillProgramBuffer("if(true)print(\"Hello, world!\")endif");

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

void TestNestedIf() {
  FillProgramBuffer(
      "if(true)\nif(false)\nprint(\"foo\")\nelse\nprint(\"bar\")"
      "\nendif\nendif");

  ConsumeNextToken();  // if
  TEST_ASSERT_EQUAL_INT(kTokenIf, token.type);

  ConsumeNextToken();  // (
  TEST_ASSERT_EQUAL_INT(kTokenLeftParenthesis, token.type);

  ConsumeNextToken();  // true
  TEST_ASSERT_EQUAL_INT(kTokenBoolean, token.type);
  TEST_ASSERT_EQUAL_INT(1, token.value.number);

  ConsumeNextToken();  // )
  TEST_ASSERT_EQUAL_INT(kTokenRightParenthesis, token.type);

  ConsumeNextToken();  // if
  TEST_ASSERT_EQUAL_INT(kTokenIf, token.type);

  ConsumeNextToken();  // (
  TEST_ASSERT_EQUAL_INT(kTokenLeftParenthesis, token.type);

  ConsumeNextToken();  // false
  TEST_ASSERT_EQUAL_INT(kTokenBoolean, token.type);
  TEST_ASSERT_EQUAL_INT(0, token.value.number);

  ConsumeNextToken();  // )
  TEST_ASSERT_EQUAL_INT(kTokenRightParenthesis, token.type);

  ConsumeNextToken();  // print
  TEST_ASSERT_EQUAL_INT(kTokenPrint, token.type);

  ConsumeNextToken();  // (
  TEST_ASSERT_EQUAL_INT(kTokenLeftParenthesis, token.type);

  ConsumeNextToken();  // foo
  TEST_ASSERT_EQUAL_INT(kTokenString, token.type);
  TEST_ASSERT_EQUAL_STRING("foo", token.value.text);

  ConsumeNextToken();  // )
  TEST_ASSERT_EQUAL_INT(kTokenRightParenthesis, token.type);

  ConsumeNextToken();  // else
  TEST_ASSERT_EQUAL_INT(kTokenElse, token.type);

  ConsumeNextToken();  // print
  TEST_ASSERT_EQUAL_INT(kTokenPrint, token.type);

  ConsumeNextToken();  // (
  TEST_ASSERT_EQUAL_INT(kTokenLeftParenthesis, token.type);

  ConsumeNextToken();  // bar
  TEST_ASSERT_EQUAL_INT(kTokenString, token.type);
  TEST_ASSERT_EQUAL_STRING("bar", token.value.text);

  ConsumeNextToken();  // )
  TEST_ASSERT_EQUAL_INT(kTokenRightParenthesis, token.type);

  ConsumeNextToken();  // endif
  TEST_ASSERT_EQUAL_INT(kTokenEndif, token.type);

  ConsumeNextToken();  // endif
  TEST_ASSERT_EQUAL_INT(kTokenEndif, token.type);

  ConsumeNextToken();  // EOF
  TEST_ASSERT_EQUAL_INT(kTokenEof, token.type);
}

void TestFor() {
  FillProgramBuffer("for(true)print(\"Hello, world!\")endfor");

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
  FillProgramBuffer("print(!true)");

  ConsumeNextToken();  // print
  ConsumeNextToken();  // (
  ConsumeNextToken();  // !
  TEST_ASSERT_EQUAL_INT(kTokenNot, token.type);

  ConsumeNextToken();  // true
  TEST_ASSERT_EQUAL_INT(kTokenBoolean, token.type);
}

void TestIntVariableDeclaration() {
  FillProgramBuffer("variable : int = 20");

  ConsumeNextToken();  // variable
  TEST_ASSERT_EQUAL_INT(kTokenIdentifier, token.type);
  TEST_ASSERT_EQUAL_STRING("variable", token.value.text);

  ConsumeNextToken();  // :
  TEST_ASSERT_EQUAL_INT(kTokenColon, token.type);

  ConsumeNextToken();  // int
  TEST_ASSERT_EQUAL_INT(kTokenInt, token.type);

  ConsumeNextToken();  // =
  TEST_ASSERT_EQUAL_INT(kTokenAssign, token.type);

  ConsumeNextToken();  // 20
  TEST_ASSERT_EQUAL_INT(kTokenNumber, token.type);
  TEST_ASSERT_EQUAL_INT(20, token.value.number);
}

void TestBoolVariableDeclaration() {
  FillProgramBuffer("variable : bool = true");

  ConsumeNextToken();  // variable
  TEST_ASSERT_EQUAL_INT(kTokenIdentifier, token.type);
  TEST_ASSERT_EQUAL_STRING("variable", token.value.text);

  ConsumeNextToken();  // :
  TEST_ASSERT_EQUAL_INT(kTokenColon, token.type);

  ConsumeNextToken();  // bool
  TEST_ASSERT_EQUAL_INT(kTokenBool, token.type);

  ConsumeNextToken();  // =
  TEST_ASSERT_EQUAL_INT(kTokenAssign, token.type);

  ConsumeNextToken();  // true
  TEST_ASSERT_EQUAL_INT(kTokenBoolean, token.type);
  TEST_ASSERT_EQUAL_INT(1, token.value.number);
}

void TestStrVariableDeclaration() {
  FillProgramBuffer("variable : str = \"foo\"");

  ConsumeNextToken();  // variable
  TEST_ASSERT_EQUAL_INT(kTokenIdentifier, token.type);
  TEST_ASSERT_EQUAL_STRING("variable", token.value.text);

  ConsumeNextToken();  // :
  TEST_ASSERT_EQUAL_INT(kTokenColon, token.type);

  ConsumeNextToken();  // str
  TEST_ASSERT_EQUAL_INT(kTokenStr, token.type);

  ConsumeNextToken();  // =
  TEST_ASSERT_EQUAL_INT(kTokenAssign, token.type);

  ConsumeNextToken();  // "foo"
  TEST_ASSERT_EQUAL_INT(kTokenString, token.type);
  TEST_ASSERT_EQUAL_STRING("foo", token.value.text);
}

void TestFloatVariableDeclaration() {
  FillProgramBuffer("variable : float = 2.2");

  ConsumeNextToken();  // variable
  TEST_ASSERT_EQUAL_INT(kTokenIdentifier, token.type);
  TEST_ASSERT_EQUAL_STRING("variable", token.value.text);

  ConsumeNextToken();  // :
  TEST_ASSERT_EQUAL_INT(kTokenColon, token.type);

  ConsumeNextToken();  // float
  TEST_ASSERT_EQUAL_INT(kTokenFloat, token.type);

  ConsumeNextToken();  // =
  TEST_ASSERT_EQUAL_INT(kTokenAssign, token.type);

  ConsumeNextToken();  // 2.2
  TEST_ASSERT_EQUAL_INT(kTokenNumber, token.type);
  TEST_ASSERT_EQUAL_INT(2, token.value.number);
}

void TestVariableAssignment() {
  FillProgramBuffer("variable = 20");

  ConsumeNextToken();  // variable
  TEST_ASSERT_EQUAL_INT(kTokenIdentifier, token.type);
  TEST_ASSERT_EQUAL_STRING("variable", token.value.text);

  ConsumeNextToken();  // =
  TEST_ASSERT_EQUAL_INT(kTokenAssign, token.type);

  ConsumeNextToken();  // 20
  TEST_ASSERT_EQUAL_INT(kTokenNumber, token.type);
  TEST_ASSERT_EQUAL_INT(20, token.value.number);
}

void TestPrintIdentifier() {
  FillProgramBuffer("print(x)");

  ConsumeNextToken();  // print
  TEST_ASSERT_EQUAL_INT(kTokenPrint, token.type);
  TEST_ASSERT_EQUAL_INT(5, program_buffer_index);

  ConsumeNextToken();  // (
  TEST_ASSERT_EQUAL_INT(kTokenLeftParenthesis, token.type);

  ConsumeNextToken();  // x
  TEST_ASSERT_EQUAL_INT(kTokenIdentifier, token.type);
  TEST_ASSERT_EQUAL_STRING("x", token.value.text);

  ConsumeNextToken();  // )
  TEST_ASSERT_EQUAL_INT(kTokenRightParenthesis, token.type);
}

void TestJumpOverNewline() {
  FillProgramBuffer("x: int = 5\nprint(x)");

  ConsumeNextToken();  // x
  TEST_ASSERT_EQUAL_INT(kTokenIdentifier, token.type);
  TEST_ASSERT_EQUAL_STRING("x", token.value.text);

  ConsumeNextToken();  // :
  TEST_ASSERT_EQUAL_INT(kTokenColon, token.type);

  ConsumeNextToken();  // int
  TEST_ASSERT_EQUAL_INT(kTokenInt, token.type);

  ConsumeNextToken();  // =
  TEST_ASSERT_EQUAL_INT(kTokenAssign, token.type);

  ConsumeNextToken();  // 5
  TEST_ASSERT_EQUAL_INT(kTokenNumber, token.type);
  TEST_ASSERT_EQUAL_INT(5, token.value.number);

  ConsumeNextToken();  // print
  TEST_ASSERT_EQUAL_INT(kTokenPrint, token.type);

  ConsumeNextToken();  // (
  TEST_ASSERT_EQUAL_INT(kTokenLeftParenthesis, token.type);

  ConsumeNextToken();  // x
  TEST_ASSERT_EQUAL_INT(kTokenIdentifier, token.type);
  TEST_ASSERT_EQUAL_STRING("x", token.value.text);

  ConsumeNextToken();  // )
  TEST_ASSERT_EQUAL_INT(kTokenRightParenthesis, token.type);
}

void TestFunctionDeclaration() {
  FillProgramBuffer("add: int = func(x: int, y: int)\nret x + y\nendfunc");

  ConsumeNextToken();  // add
  TEST_ASSERT_EQUAL_INT(kTokenIdentifier, token.type);
  TEST_ASSERT_EQUAL_STRING("add", token.value.text);

  ConsumeNextToken();  // :
  TEST_ASSERT_EQUAL_INT(kTokenColon, token.type);

  ConsumeNextToken();  // int
  TEST_ASSERT_EQUAL_INT(kTokenInt, token.type);

  ConsumeNextToken();  // =
  TEST_ASSERT_EQUAL_INT(kTokenAssign, token.type);

  ConsumeNextToken();  // func
  TEST_ASSERT_EQUAL_INT(kTokenFunc, token.type);

  ConsumeNextToken();  // (
  TEST_ASSERT_EQUAL_INT(kTokenLeftParenthesis, token.type);

  ConsumeNextToken();  // x
  TEST_ASSERT_EQUAL_INT(kTokenIdentifier, token.type);
  TEST_ASSERT_EQUAL_STRING("x", token.value.text);

  ConsumeNextToken();  // :
  TEST_ASSERT_EQUAL_INT(kTokenColon, token.type);

  ConsumeNextToken();  // int
  TEST_ASSERT_EQUAL_INT(kTokenInt, token.type);

  ConsumeNextToken();  // ,
  TEST_ASSERT_EQUAL_INT(kTokenComma, token.type);

  ConsumeNextToken();  // y
  TEST_ASSERT_EQUAL_INT(kTokenIdentifier, token.type);
  TEST_ASSERT_EQUAL_STRING("y", token.value.text);

  ConsumeNextToken();  // :
  TEST_ASSERT_EQUAL_INT(kTokenColon, token.type);

  ConsumeNextToken();  // int
  TEST_ASSERT_EQUAL_INT(kTokenInt, token.type);

  ConsumeNextToken();  // )
  TEST_ASSERT_EQUAL_INT(kTokenRightParenthesis, token.type);

  ConsumeNextToken();  // ret
  TEST_ASSERT_EQUAL_INT(kTokenRet, token.type);

  ConsumeNextToken();  // x
  TEST_ASSERT_EQUAL_INT(kTokenIdentifier, token.type);
  TEST_ASSERT_EQUAL_STRING("x", token.value.text);

  ConsumeNextToken();  // +
  TEST_ASSERT_EQUAL_INT(kTokenPlus, token.type);

  ConsumeNextToken();  // y
  TEST_ASSERT_EQUAL_INT(kTokenIdentifier, token.type);
  TEST_ASSERT_EQUAL_STRING("y", token.value.text);

  ConsumeNextToken();  // endfunc
  TEST_ASSERT_EQUAL_INT(kTokenEndfunc, token.type);
}

void TestFunctionDeclarationPrintAndCall() {
  FillProgramBuffer(
      "add: int = func(x: int, y: int)\nret x + y\nendfunc\nprint(add(5, 6))");

  ConsumeNextToken();  // add
  TEST_ASSERT_EQUAL_INT(kTokenIdentifier, token.type);
  TEST_ASSERT_EQUAL_STRING("add", token.value.text);

  ConsumeNextToken();  // :
  TEST_ASSERT_EQUAL_INT(kTokenColon, token.type);

  ConsumeNextToken();  // int
  TEST_ASSERT_EQUAL_INT(kTokenInt, token.type);

  ConsumeNextToken();  // =
  TEST_ASSERT_EQUAL_INT(kTokenAssign, token.type);

  ConsumeNextToken();  // func
  TEST_ASSERT_EQUAL_INT(kTokenFunc, token.type);

  ConsumeNextToken();  // (
  TEST_ASSERT_EQUAL_INT(kTokenLeftParenthesis, token.type);

  ConsumeNextToken();  // x
  TEST_ASSERT_EQUAL_INT(kTokenIdentifier, token.type);
  TEST_ASSERT_EQUAL_STRING("x", token.value.text);

  ConsumeNextToken();  // :
  TEST_ASSERT_EQUAL_INT(kTokenColon, token.type);

  ConsumeNextToken();  // int
  TEST_ASSERT_EQUAL_INT(kTokenInt, token.type);

  ConsumeNextToken();  // ,
  TEST_ASSERT_EQUAL_INT(kTokenComma, token.type);

  ConsumeNextToken();  // y
  TEST_ASSERT_EQUAL_INT(kTokenIdentifier, token.type);
  TEST_ASSERT_EQUAL_STRING("y", token.value.text);

  ConsumeNextToken();  // :
  TEST_ASSERT_EQUAL_INT(kTokenColon, token.type);

  ConsumeNextToken();  // int
  TEST_ASSERT_EQUAL_INT(kTokenInt, token.type);

  ConsumeNextToken();  // )
  TEST_ASSERT_EQUAL_INT(kTokenRightParenthesis, token.type);

  ConsumeNextToken();  // ret
  TEST_ASSERT_EQUAL_INT(kTokenRet, token.type);

  ConsumeNextToken();  // x
  TEST_ASSERT_EQUAL_INT(kTokenIdentifier, token.type);
  TEST_ASSERT_EQUAL_STRING("x", token.value.text);

  ConsumeNextToken();  // +
  TEST_ASSERT_EQUAL_INT(kTokenPlus, token.type);

  ConsumeNextToken();  // y
  TEST_ASSERT_EQUAL_INT(kTokenIdentifier, token.type);
  TEST_ASSERT_EQUAL_STRING("y", token.value.text);

  ConsumeNextToken();  // endfunc
  TEST_ASSERT_EQUAL_INT(kTokenEndfunc, token.type);

  ConsumeNextToken();  // print
  TEST_ASSERT_EQUAL_INT(kTokenPrint, token.type);

  ConsumeNextToken();  // (
  TEST_ASSERT_EQUAL_INT(kTokenLeftParenthesis, token.type);

  ConsumeNextToken();  // add
  TEST_ASSERT_EQUAL_INT(kTokenIdentifier, token.type);
  TEST_ASSERT_EQUAL_STRING("add", token.value.text);

  ConsumeNextToken();  // (
  TEST_ASSERT_EQUAL_INT(kTokenLeftParenthesis, token.type);

  ConsumeNextToken();  // 5
  TEST_ASSERT_EQUAL_INT(kTokenNumber, token.type);
  TEST_ASSERT_EQUAL_INT(5, token.value.number);

  ConsumeNextToken();  // ,
  TEST_ASSERT_EQUAL_INT(kTokenComma, token.type);

  ConsumeNextToken();  // 6
  TEST_ASSERT_EQUAL_INT(kTokenNumber, token.type);
  TEST_ASSERT_EQUAL_INT(6, token.value.number);

  ConsumeNextToken();  // )
  TEST_ASSERT_EQUAL_INT(kTokenRightParenthesis, token.type);

  ConsumeNextToken();  // )
  TEST_ASSERT_EQUAL_INT(kTokenRightParenthesis, token.type);
}

void TestFunctionCallComma() {
  FillProgramBuffer("add(5, 6)");

  ConsumeNextToken();  // add
  TEST_ASSERT_EQUAL_INT(kTokenIdentifier, token.type);
  TEST_ASSERT_EQUAL_STRING("add", token.value.text);

  ConsumeNextToken();  // (
  TEST_ASSERT_EQUAL_INT(kTokenLeftParenthesis, token.type);

  ConsumeNextToken();  // 5
  TEST_ASSERT_EQUAL_INT(kTokenNumber, token.type);
  TEST_ASSERT_EQUAL_INT(5, token.value.number);

  ConsumeNextToken();  // ,
  TEST_ASSERT_EQUAL_INT(kTokenComma, token.type);

  ConsumeNextToken();  // 6
  TEST_ASSERT_EQUAL_INT(kTokenNumber, token.type);
  TEST_ASSERT_EQUAL_INT(6, token.value.number);

  ConsumeNextToken();  // )
  TEST_ASSERT_EQUAL_INT(kTokenRightParenthesis, token.type);
}

void TestFunctionCallInPrint() {
  FillProgramBuffer("print(add(5, 6))");

  ConsumeNextToken();  // print
  TEST_ASSERT_EQUAL_INT(kTokenPrint, token.type);

  ConsumeNextToken();  // (
  TEST_ASSERT_EQUAL_INT(kTokenLeftParenthesis, token.type);

  ConsumeNextToken();  // add
  TEST_ASSERT_EQUAL_INT(kTokenIdentifier, token.type);
  TEST_ASSERT_EQUAL_STRING("add", token.value.text);

  ConsumeNextToken();  // (
  TEST_ASSERT_EQUAL_INT(kTokenLeftParenthesis, token.type);

  ConsumeNextToken();  // 5
  TEST_ASSERT_EQUAL_INT(kTokenNumber, token.type);
  TEST_ASSERT_EQUAL_INT(5, token.value.number);

  ConsumeNextToken();  // ,
  TEST_ASSERT_EQUAL_INT(kTokenComma, token.type);

  ConsumeNextToken();  // 6
  TEST_ASSERT_EQUAL_INT(kTokenNumber, token.type);
  TEST_ASSERT_EQUAL_INT(6, token.value.number);

  ConsumeNextToken();  // )
  TEST_ASSERT_EQUAL_INT(kTokenRightParenthesis, token.type);

  ConsumeNextToken();  // )
  TEST_ASSERT_EQUAL_INT(kTokenRightParenthesis, token.type);
}
