/*
   Copyright 2020 Alexandre Pires (c.alexandre.pires@gmail.com)

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/
#include "gtest/gtest.h"
#include "Script/Parser/Lexer.h"

#define OP_TEST_IDENTIFIER(x, l, o, r)                         \
    {                                                          \
        std::string expr = x;                                  \
        Antomic::Lexer lex(expr, "expression");                \
        auto left = lex.Read();                                \
        auto op = lex.Read();                                  \
        auto right = lex.Read();                               \
        EXPECT_EQ(left.Type, Antomic::TokenType::Identifier);  \
        EXPECT_EQ(left.Value, l);                              \
        EXPECT_EQ(op.Type, o);                                 \
        EXPECT_EQ(right.Type, Antomic::TokenType::Identifier); \
        EXPECT_EQ(right.Value, r);                             \
    }

#define TYPE_TEST(x, l)                         \
    {                                           \
        std::string expr = x;                   \
        Antomic::Lexer lex(expr, "expression"); \
        auto left = lex.Read();                 \
        EXPECT_EQ(left.Type, l);                \
    }

#define TEST_TOKEN_LC_VALUE(lex, type, value, line, col) \
    {                                                    \
        auto t = lex.Read();                             \
        EXPECT_EQ(t.Type, type);                         \
        EXPECT_EQ(t.Value, value);                       \
        EXPECT_EQ(t.Line, line);                         \
        EXPECT_EQ(t.Column, col);                        \
    }

#define TEST_TOKEN_LC(lex, type, line, col) \
    {                                       \
        auto t = lex.Read();                \
        EXPECT_EQ(t.Type, type);            \
        EXPECT_EQ(t.Line, line);            \
        EXPECT_EQ(t.Column, col);           \
    }

#define TEST_TOKEN_VALUE(lex, type, value) \
    {                                      \
        auto t = lex.Read();               \
        EXPECT_EQ(t.Type, type);           \
        EXPECT_EQ(t.Value, value);         \
    }

#define TEST_TOKEN(lex, type)    \
    {                            \
        auto t = lex.Read();     \
        EXPECT_EQ(t.Type, type); \
    }

#define TEST_NEWLINE(lex)                               \
    {                                                   \
        auto t = lex.Read();                            \
        EXPECT_EQ(t.Type, Antomic::TokenType::NewLine); \
    }

#define TEST_END(lex)                               \
    {                                               \
        auto t = lex.Read();                        \
        EXPECT_EQ(t.Type, Antomic::TokenType::End); \
    }

static std::map<std::string, Antomic::TokenType> sKeywordsMaps{
    {"and", Antomic::TokenType::KeywordAnd},
    {"as", Antomic::TokenType::KeywordAs},
    {"assert", Antomic::TokenType::KeywordAssert},
    {"break", Antomic::TokenType::KeywordBreak},
    {"class", Antomic::TokenType::KeywordClass},
    {"continue", Antomic::TokenType::KeywordContinue},
    {"def", Antomic::TokenType::KeywordDef},
    {"del", Antomic::TokenType::KeywordDel},
    {"return", Antomic::TokenType::KeywordReturn},
    {"elif", Antomic::TokenType::KeywordElIf},
    {"else", Antomic::TokenType::KeywordElse},
    {"except", Antomic::TokenType::KeywordExcept},
    {"exec", Antomic::TokenType::KeywordExec},
    {"False", Antomic::TokenType::KeywordFalse},
    {"finally", Antomic::TokenType::KeywordFinally},
    {"for", Antomic::TokenType::KeywordFor},
    {"from", Antomic::TokenType::KeywordFrom},
    {"if", Antomic::TokenType::KeywordIf},
    {"import", Antomic::TokenType::KeywordImport},
    {"in", Antomic::TokenType::KeywordIn},
    {"is", Antomic::TokenType::KeywordIs},
    {"lambda", Antomic::TokenType::KeywordLambda},
    {"None", Antomic::TokenType::KeywordNone},
    {"not", Antomic::TokenType::KeywordNot},
    {"or", Antomic::TokenType::KeywordOr},
    {"pass", Antomic::TokenType::KeywordPass},
    {"raise", Antomic::TokenType::KeywordRaise},
    {"True", Antomic::TokenType::KeywordTrue},
    {"try", Antomic::TokenType::KeywordTry},
    {"while", Antomic::TokenType::KeywordWhile},
};

TEST(AntomicCoreTest, LexerOperatorTests)
{
    OP_TEST_IDENTIFIER("a-b", "a", Antomic::TokenType::OpSub, "b");
    OP_TEST_IDENTIFIER("a - b", "a", Antomic::TokenType::OpSub, "b");
    OP_TEST_IDENTIFIER("a+b", "a", Antomic::TokenType::OpAdd, "b");
    OP_TEST_IDENTIFIER("a + b", "a", Antomic::TokenType::OpAdd, "b");
    OP_TEST_IDENTIFIER("a*b", "a", Antomic::TokenType::OpMul, "b");
    OP_TEST_IDENTIFIER("a * b", "a", Antomic::TokenType::OpMul, "b");
    OP_TEST_IDENTIFIER("a/b", "a", Antomic::TokenType::OpDiv, "b");
    OP_TEST_IDENTIFIER("a / b", "a", Antomic::TokenType::OpDiv, "b");
    OP_TEST_IDENTIFIER("a**b", "a", Antomic::TokenType::OpExp, "b");
    OP_TEST_IDENTIFIER("a ** b", "a", Antomic::TokenType::OpExp, "b");
    OP_TEST_IDENTIFIER("a%b", "a", Antomic::TokenType::OpMod, "b");
    OP_TEST_IDENTIFIER("a % b", "a", Antomic::TokenType::OpMod, "b");
    OP_TEST_IDENTIFIER("a&b", "a", Antomic::TokenType::OpAnd, "b");
    OP_TEST_IDENTIFIER("a & b", "a", Antomic::TokenType::OpAnd, "b");
    OP_TEST_IDENTIFIER("a|b", "a", Antomic::TokenType::OpOr, "b");
    OP_TEST_IDENTIFIER("a | b", "a", Antomic::TokenType::OpOr, "b");
    OP_TEST_IDENTIFIER("a^b", "a", Antomic::TokenType::OpXor, "b");
    OP_TEST_IDENTIFIER("a ^ b", "a", Antomic::TokenType::OpXor, "b");
    OP_TEST_IDENTIFIER("a//b", "a", Antomic::TokenType::OpFloorDiv, "b");
    OP_TEST_IDENTIFIER("a // b", "a", Antomic::TokenType::OpFloorDiv, "b");
    OP_TEST_IDENTIFIER("a<<b", "a", Antomic::TokenType::OpShiftLeft, "b");
    OP_TEST_IDENTIFIER("a << b", "a", Antomic::TokenType::OpShiftLeft, "b");
    OP_TEST_IDENTIFIER("a>>b", "a", Antomic::TokenType::OpShiftRight, "b");
    OP_TEST_IDENTIFIER("a >> b", "a", Antomic::TokenType::OpShiftRight, "b");
    OP_TEST_IDENTIFIER("a==b", "a", Antomic::TokenType::OpEqual, "b");
    OP_TEST_IDENTIFIER("a == b", "a", Antomic::TokenType::OpEqual, "b");
    OP_TEST_IDENTIFIER("a!=b", "a", Antomic::TokenType::OpNotEqual, "b");
    OP_TEST_IDENTIFIER("a != b", "a", Antomic::TokenType::OpNotEqual, "b");
    OP_TEST_IDENTIFIER("a>=b", "a", Antomic::TokenType::OpGreatEqual, "b");
    OP_TEST_IDENTIFIER("a >=b ", "a", Antomic::TokenType::OpGreatEqual, "b");
    OP_TEST_IDENTIFIER("a<=b", "a", Antomic::TokenType::OpLessEqual, "b");
    OP_TEST_IDENTIFIER("a <=b ", "a", Antomic::TokenType::OpLessEqual, "b");
    OP_TEST_IDENTIFIER("a>b", "a", Antomic::TokenType::OpGreat, "b");
    OP_TEST_IDENTIFIER("a > b", "a", Antomic::TokenType::OpGreat, "b");
    OP_TEST_IDENTIFIER("a<b", "a", Antomic::TokenType::OpLess, "b");
    OP_TEST_IDENTIFIER("a < b", "a", Antomic::TokenType::OpLess, "b");
    OP_TEST_IDENTIFIER("a=b", "a", Antomic::TokenType::OpAssign, "b");
    OP_TEST_IDENTIFIER("a = b", "a", Antomic::TokenType::OpAssign, "b");
    OP_TEST_IDENTIFIER("a+=b", "a", Antomic::TokenType::OpAddAssign, "b");
    OP_TEST_IDENTIFIER("a += b", "a", Antomic::TokenType::OpAddAssign, "b");
    OP_TEST_IDENTIFIER("a-=b", "a", Antomic::TokenType::OpSubAssign, "b");
    OP_TEST_IDENTIFIER("a -= b", "a", Antomic::TokenType::OpSubAssign, "b");
    OP_TEST_IDENTIFIER("a*=b", "a", Antomic::TokenType::OpMulAssign, "b");
    OP_TEST_IDENTIFIER("a *= b", "a", Antomic::TokenType::OpMulAssign, "b");
    OP_TEST_IDENTIFIER("a/=b", "a", Antomic::TokenType::OpDivAssign, "b");
    OP_TEST_IDENTIFIER("a /= b", "a", Antomic::TokenType::OpDivAssign, "b");
    OP_TEST_IDENTIFIER("a//=b", "a", Antomic::TokenType::OpFloorDivAssign, "b");
    OP_TEST_IDENTIFIER("a //= b", "a", Antomic::TokenType::OpFloorDivAssign, "b");
    OP_TEST_IDENTIFIER("a%=b", "a", Antomic::TokenType::OpModAssign, "b");
    OP_TEST_IDENTIFIER("a %= b", "a", Antomic::TokenType::OpModAssign, "b");
    OP_TEST_IDENTIFIER("a&=b", "a", Antomic::TokenType::OpAndAssign, "b");
    OP_TEST_IDENTIFIER("a &= b", "a", Antomic::TokenType::OpAndAssign, "b");
    OP_TEST_IDENTIFIER("a|=b", "a", Antomic::TokenType::OpOrAssign, "b");
    OP_TEST_IDENTIFIER("a |= b", "a", Antomic::TokenType::OpOrAssign, "b");
    OP_TEST_IDENTIFIER("a^=b", "a", Antomic::TokenType::OpXorAssign, "b");
    OP_TEST_IDENTIFIER("a ^=b ", "a", Antomic::TokenType::OpXorAssign, "b");
    OP_TEST_IDENTIFIER("a<<=b", "a", Antomic::TokenType::OpShiftLeftAssign, "b");
    OP_TEST_IDENTIFIER("a <<= b", "a", Antomic::TokenType::OpShiftLeftAssign, "b");
    OP_TEST_IDENTIFIER("a>>=b", "a", Antomic::TokenType::OpShiftRightAssign, "b");
    OP_TEST_IDENTIFIER("a >>= b", "a", Antomic::TokenType::OpShiftRightAssign, "b");
    OP_TEST_IDENTIFIER("a**=b", "a", Antomic::TokenType::OpExpAssign, "b");
    OP_TEST_IDENTIFIER("a **= b", "a", Antomic::TokenType::OpExpAssign, "b");
}

TEST(AntomicCoreTest, LexerSymbolTests)
{
    TYPE_TEST("[", Antomic::TokenType::SymbolBracketOpen);
    TYPE_TEST("]", Antomic::TokenType::SymbolBracketClose);
    TYPE_TEST(",", Antomic::TokenType::SymbolComma);
    TYPE_TEST(":", Antomic::TokenType::SymbolColon);
    TYPE_TEST(".", Antomic::TokenType::SymbolPeriod);
    TYPE_TEST(";", Antomic::TokenType::SymbolSemiColon);
    TYPE_TEST("{", Antomic::TokenType::SymbolBraceOpen);
    TYPE_TEST("}", Antomic::TokenType::SymbolBraceClose);
    TYPE_TEST("(", Antomic::TokenType::SymbolParentesesOpen);
    TYPE_TEST(")", Antomic::TokenType::SymbolParentesesClose);
}

TEST(AntomicCoreTest, LexerKeywordTests)
{
    for (auto keyword : sKeywordsMaps)
    {
        TYPE_TEST(keyword.first, keyword.second);
    }
}

TEST(AntomicCoreTest, LexerFromFileTests)
{
    Antomic::Lexer lex("tests/files/lexer_test.py");

    // # A commment
    TEST_TOKEN_LC_VALUE(lex, Antomic::TokenType::Comment, "# A commment", 1, 1);
    TEST_TOKEN(lex, Antomic::TokenType::NewLine);

    // a = "a string"
    TEST_TOKEN(lex, Antomic::TokenType::Identation);
    TEST_TOKEN_LC_VALUE(lex, Antomic::TokenType::Identifier, "a", 2, 1);
    TEST_TOKEN_LC(lex, Antomic::TokenType::OpAssign, 2, 3);
    TEST_TOKEN_LC_VALUE(lex, Antomic::TokenType::String, "a string", 2, 5);
    TEST_NEWLINE(lex);

    // print(" a multiline string
    //     asd")
    TEST_TOKEN(lex, Antomic::TokenType::Identation);
    TEST_TOKEN_LC_VALUE(lex, Antomic::TokenType::Identifier, "print", 3, 1);
    TEST_TOKEN_LC(lex, Antomic::TokenType::SymbolParentesesOpen, 3, 6);
    TEST_TOKEN_LC_VALUE(lex, Antomic::TokenType::String, " a multiline string \n    asd", 3, 7);
    TEST_TOKEN_LC(lex, Antomic::TokenType::SymbolParentesesClose, 4, 9);
    TEST_NEWLINE(lex);

    // b='another string'
    TEST_TOKEN(lex, Antomic::TokenType::Identation);
    TEST_TOKEN_LC_VALUE(lex, Antomic::TokenType::Identifier, "b", 6, 1);
    TEST_TOKEN_LC(lex, Antomic::TokenType::OpAssign, 6, 2);
    TEST_TOKEN_LC_VALUE(lex, Antomic::TokenType::String, "another string", 6, 3);
    TEST_NEWLINE(lex);

    // print(b)
    TEST_TOKEN(lex, Antomic::TokenType::Identation);
    TEST_TOKEN_LC_VALUE(lex, Antomic::TokenType::Identifier, "print", 8, 1);
    TEST_TOKEN_LC(lex, Antomic::TokenType::SymbolParentesesOpen, 8, 6);
    TEST_TOKEN_LC_VALUE(lex, Antomic::TokenType::Identifier, "b", 8, 7);
    TEST_TOKEN_LC(lex, Antomic::TokenType::SymbolParentesesClose, 8, 8);
    TEST_NEWLINE(lex);

    // for a in range(1):
    //     b = a
    TEST_TOKEN(lex, Antomic::TokenType::Identation);
    TEST_TOKEN(lex, Antomic::TokenType::KeywordFor);
    TEST_TOKEN_VALUE(lex, Antomic::TokenType::Identifier, "a");
    TEST_TOKEN(lex, Antomic::TokenType::KeywordIn);
    TEST_TOKEN_VALUE(lex, Antomic::TokenType::Identifier, "range");
    TEST_TOKEN(lex, Antomic::TokenType::SymbolParentesesOpen);
    TEST_TOKEN_VALUE(lex, Antomic::TokenType::NumberInteger, "1");
    TEST_TOKEN(lex, Antomic::TokenType::SymbolParentesesClose);
    TEST_TOKEN(lex, Antomic::TokenType::SymbolColon);
    TEST_NEWLINE(lex);
    TEST_TOKEN(lex, Antomic::TokenType::Identation);
    TEST_TOKEN_VALUE(lex, Antomic::TokenType::Identifier, "b");
    TEST_TOKEN(lex, Antomic::TokenType::OpAssign);
    TEST_TOKEN_VALUE(lex, Antomic::TokenType::Identifier, "a");
    TEST_NEWLINE(lex);

    // c = 12.0 + 
    //     0x32
    TEST_TOKEN(lex, Antomic::TokenType::Identation);
    TEST_TOKEN_VALUE(lex, Antomic::TokenType::Identifier, "c");
    TEST_TOKEN(lex, Antomic::TokenType::OpAssign);
    TEST_TOKEN_VALUE(lex, Antomic::TokenType::NumberFloat, "12.0");
    TEST_TOKEN(lex, Antomic::TokenType::OpAdd);
    TEST_TOKEN_VALUE(lex, Antomic::TokenType::NumberHex, "0x32");
    TEST_NEWLINE(lex);

    // def method():
    //     pass
    TEST_TOKEN(lex, Antomic::TokenType::Identation);
    TEST_TOKEN(lex, Antomic::TokenType::KeywordDef);
    TEST_TOKEN_VALUE(lex, Antomic::TokenType::Identifier, "method");
    TEST_TOKEN(lex, Antomic::TokenType::SymbolParentesesOpen);
    TEST_TOKEN(lex, Antomic::TokenType::SymbolParentesesClose);
    TEST_TOKEN(lex, Antomic::TokenType::SymbolColon);
    TEST_NEWLINE(lex);
    TEST_TOKEN(lex, Antomic::TokenType::Identation);
    TEST_TOKEN(lex, Antomic::TokenType::KeywordPass);
    TEST_NEWLINE(lex);

    // .9
    TEST_TOKEN(lex, Antomic::TokenType::Identation);
    TEST_TOKEN_VALUE(lex, Antomic::TokenType::NumberFloat, ".9");
    TEST_NEWLINE(lex);

    // c = method()
    TEST_TOKEN(lex, Antomic::TokenType::Identation);
    TEST_TOKEN_VALUE(lex, Antomic::TokenType::Identifier, "c");
    TEST_TOKEN(lex, Antomic::TokenType::OpAssign);
    TEST_TOKEN_VALUE(lex, Antomic::TokenType::Identifier, "method");
    TEST_TOKEN(lex, Antomic::TokenType::SymbolParentesesOpen);
    TEST_TOKEN(lex, Antomic::TokenType::SymbolParentesesClose);
    TEST_NEWLINE(lex);

    // def method(a,b,c):
    //     pass
    TEST_TOKEN(lex, Antomic::TokenType::Identation);
    TEST_TOKEN(lex, Antomic::TokenType::KeywordDef);
    TEST_TOKEN_VALUE(lex, Antomic::TokenType::Identifier, "method");
    TEST_TOKEN(lex, Antomic::TokenType::SymbolParentesesOpen);
    TEST_TOKEN_VALUE(lex, Antomic::TokenType::Identifier, "a");
    TEST_TOKEN(lex, Antomic::TokenType::SymbolComma);
    TEST_TOKEN_VALUE(lex, Antomic::TokenType::Identifier, "b");
    TEST_TOKEN(lex, Antomic::TokenType::SymbolComma);
    TEST_TOKEN_VALUE(lex, Antomic::TokenType::Identifier, "c");
    TEST_TOKEN(lex, Antomic::TokenType::SymbolParentesesClose);
    TEST_TOKEN(lex, Antomic::TokenType::SymbolColon);
    TEST_NEWLINE(lex);
    TEST_TOKEN(lex, Antomic::TokenType::Identation);
    TEST_TOKEN(lex, Antomic::TokenType::KeywordPass);
    TEST_NEWLINE(lex);

    // method(1,2,a)
    TEST_TOKEN(lex, Antomic::TokenType::Identation);
    TEST_TOKEN_VALUE(lex, Antomic::TokenType::Identifier, "method");
    TEST_TOKEN(lex, Antomic::TokenType::SymbolParentesesOpen);
    TEST_TOKEN_VALUE(lex, Antomic::TokenType::NumberInteger, "1");
    TEST_TOKEN(lex, Antomic::TokenType::SymbolComma);
    TEST_TOKEN_VALUE(lex, Antomic::TokenType::NumberInteger, "2");
    TEST_TOKEN(lex, Antomic::TokenType::SymbolComma);
    TEST_TOKEN_VALUE(lex, Antomic::TokenType::Identifier, "a");
    TEST_TOKEN(lex, Antomic::TokenType::SymbolParentesesClose);
    TEST_NEWLINE(lex);

    // (a,b) = (2,3)
    TEST_TOKEN(lex, Antomic::TokenType::Identation);
    TEST_TOKEN(lex, Antomic::TokenType::SymbolParentesesOpen);
    TEST_TOKEN_VALUE(lex, Antomic::TokenType::Identifier, "a");
    TEST_TOKEN(lex, Antomic::TokenType::SymbolComma);
    TEST_TOKEN_VALUE(lex, Antomic::TokenType::Identifier, "b");
    TEST_TOKEN(lex, Antomic::TokenType::SymbolParentesesClose);
    TEST_TOKEN(lex, Antomic::TokenType::OpAssign);
    TEST_TOKEN(lex, Antomic::TokenType::SymbolParentesesOpen);
    TEST_TOKEN_VALUE(lex, Antomic::TokenType::NumberInteger, "2");
    TEST_TOKEN(lex, Antomic::TokenType::SymbolComma);
    TEST_TOKEN_VALUE(lex, Antomic::TokenType::NumberInteger, "3");
    TEST_TOKEN(lex, Antomic::TokenType::SymbolParentesesClose);
    TEST_NEWLINE(lex);

    // a = {
    //     "a" : 1,
    //     "b" : 0.1,
    //     "c" : True,
    //     "d" : False
    // }
    TEST_TOKEN(lex, Antomic::TokenType::Identation);
    TEST_TOKEN_VALUE(lex, Antomic::TokenType::Identifier, "a");
    TEST_TOKEN(lex, Antomic::TokenType::OpAssign);
    TEST_TOKEN(lex, Antomic::TokenType::SymbolBraceOpen);
    TEST_NEWLINE(lex);

    TEST_TOKEN(lex, Antomic::TokenType::Identation);
    TEST_TOKEN_VALUE(lex, Antomic::TokenType::String, "a");
    TEST_TOKEN(lex, Antomic::TokenType::SymbolColon);
    TEST_TOKEN_VALUE(lex, Antomic::TokenType::NumberInteger, "1");
    TEST_TOKEN(lex, Antomic::TokenType::SymbolComma);
    TEST_NEWLINE(lex);

    TEST_TOKEN(lex, Antomic::TokenType::Identation);
    TEST_TOKEN_VALUE(lex, Antomic::TokenType::String, "b");
    TEST_TOKEN(lex, Antomic::TokenType::SymbolColon);
    TEST_TOKEN_VALUE(lex, Antomic::TokenType::NumberFloat, "0.1");
    TEST_TOKEN(lex, Antomic::TokenType::SymbolComma);
    TEST_NEWLINE(lex);

    TEST_TOKEN(lex, Antomic::TokenType::Identation);
    TEST_TOKEN_VALUE(lex, Antomic::TokenType::String, "c");
    TEST_TOKEN(lex, Antomic::TokenType::SymbolColon);
    TEST_TOKEN(lex, Antomic::TokenType::KeywordTrue);
    TEST_TOKEN(lex, Antomic::TokenType::SymbolComma);
    TEST_NEWLINE(lex);

    TEST_TOKEN(lex, Antomic::TokenType::Identation);
    TEST_TOKEN_VALUE(lex, Antomic::TokenType::String, "d");
    TEST_TOKEN(lex, Antomic::TokenType::SymbolColon);
    TEST_TOKEN(lex, Antomic::TokenType::KeywordFalse);
    TEST_NEWLINE(lex);

    TEST_TOKEN(lex, Antomic::TokenType::Identation);
    TEST_TOKEN(lex, Antomic::TokenType::SymbolBraceClose);
    TEST_NEWLINE(lex);

    // b = [ 1, "a", None, True, False, (1000,2000) ]
    TEST_TOKEN(lex, Antomic::TokenType::Identation);
    TEST_TOKEN_VALUE(lex, Antomic::TokenType::Identifier, "b");
    TEST_TOKEN(lex, Antomic::TokenType::OpAssign);
    TEST_TOKEN(lex, Antomic::TokenType::SymbolBracketOpen);
    TEST_TOKEN_VALUE(lex, Antomic::TokenType::NumberInteger, "1");
    TEST_TOKEN(lex, Antomic::TokenType::SymbolComma);
    TEST_TOKEN_VALUE(lex, Antomic::TokenType::String, "a");
    TEST_TOKEN(lex, Antomic::TokenType::SymbolComma);
    TEST_TOKEN(lex, Antomic::TokenType::KeywordNone);
    TEST_TOKEN(lex, Antomic::TokenType::SymbolComma);
    TEST_TOKEN(lex, Antomic::TokenType::KeywordTrue);
    TEST_TOKEN(lex, Antomic::TokenType::SymbolComma);
    TEST_TOKEN(lex, Antomic::TokenType::KeywordFalse);
    TEST_TOKEN(lex, Antomic::TokenType::SymbolComma);
    TEST_TOKEN(lex, Antomic::TokenType::SymbolParentesesOpen);
    TEST_TOKEN_VALUE(lex, Antomic::TokenType::NumberInteger, "1000");
    TEST_TOKEN(lex, Antomic::TokenType::SymbolComma);
    TEST_TOKEN_VALUE(lex, Antomic::TokenType::NumberInteger, "2000");
    TEST_TOKEN(lex, Antomic::TokenType::SymbolParentesesClose);
    TEST_TOKEN(lex, Antomic::TokenType::SymbolBracketClose);
    TEST_NEWLINE(lex);

    // class A(base1, base2):
    //     def __init__(self):
    //         pass
    TEST_TOKEN(lex, Antomic::TokenType::Identation);
    TEST_TOKEN(lex, Antomic::TokenType::KeywordClass);
    TEST_TOKEN_VALUE(lex, Antomic::TokenType::Identifier, "A");
    TEST_TOKEN(lex, Antomic::TokenType::SymbolParentesesOpen);
    TEST_TOKEN_VALUE(lex, Antomic::TokenType::Identifier, "base1");
    TEST_TOKEN(lex, Antomic::TokenType::SymbolComma);
    TEST_TOKEN_VALUE(lex, Antomic::TokenType::Identifier, "base2");
    TEST_TOKEN(lex, Antomic::TokenType::SymbolParentesesClose);
    TEST_TOKEN(lex, Antomic::TokenType::SymbolColon);
    TEST_NEWLINE(lex);
    TEST_TOKEN(lex, Antomic::TokenType::Identation);
    TEST_TOKEN(lex, Antomic::TokenType::KeywordDef);
    TEST_TOKEN_VALUE(lex, Antomic::TokenType::Identifier, "__init__");
    TEST_TOKEN(lex, Antomic::TokenType::SymbolParentesesOpen);
    TEST_TOKEN_VALUE(lex, Antomic::TokenType::Identifier, "self");
    TEST_TOKEN(lex, Antomic::TokenType::SymbolParentesesClose);
    TEST_TOKEN(lex, Antomic::TokenType::SymbolColon);
    TEST_NEWLINE(lex);
    TEST_TOKEN(lex, Antomic::TokenType::Identation);
    TEST_TOKEN(lex, Antomic::TokenType::KeywordPass);
    TEST_END(lex);
}
