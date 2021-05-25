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
#include "Python/Parser/Lexer.h"

TEST(AntomicCoreTest, PythonTests)
{
    Antomic::Lexer lex("tests/files/lexer_test.py");

    Antomic::Token t = lex.Next();
    EXPECT_EQ(t.Type, Antomic::TokenType::Comment);
    EXPECT_EQ(t.Value, "# A commment");
    EXPECT_EQ(t.Line, 1);
    EXPECT_EQ(t.Column, 1);

    t = lex.Next();
    EXPECT_EQ(t.Type, Antomic::TokenType::Identifier);
    EXPECT_EQ(t.Value, "a");
    EXPECT_EQ(t.Line, 2);
    EXPECT_EQ(t.Column, 1);

    t = lex.Next();
    EXPECT_EQ(t.Type, Antomic::TokenType::OpAssign);
    EXPECT_EQ(t.Line, 2);
    EXPECT_EQ(t.Column, 3);

    t = lex.Next();
    EXPECT_EQ(t.Type, Antomic::TokenType::String);
    EXPECT_EQ(t.Value, "a string");
    EXPECT_EQ(t.Line, 2);
    EXPECT_EQ(t.Column, 5);

    t = lex.Next();
    EXPECT_EQ(t.Type, Antomic::TokenType::Identifier);
    EXPECT_EQ(t.Value, "print");
    EXPECT_EQ(t.Line, 3);
    EXPECT_EQ(t.Column, 1);

    t = lex.Next();
    EXPECT_EQ(t.Type, Antomic::TokenType::SymbolParentesesOpen);
    EXPECT_EQ(t.Line, 3);
    EXPECT_EQ(t.Column, 6);

    t = lex.Next();
    EXPECT_EQ(t.Type, Antomic::TokenType::String);
    EXPECT_EQ(t.Value, " a multiline string \n    asd");
    EXPECT_EQ(t.Line, 3);
    EXPECT_EQ(t.Column, 7);

    t = lex.Next();
    EXPECT_EQ(t.Type, Antomic::TokenType::SymbolParentesesClose);
    EXPECT_EQ(t.Line, 4);
    EXPECT_EQ(t.Column, 9);

    t = lex.Next();
    EXPECT_EQ(t.Type, Antomic::TokenType::Identifier);
    EXPECT_EQ(t.Value, "b");
    EXPECT_EQ(t.Line, 6);
    EXPECT_EQ(t.Column, 1);

    t = lex.Next();
    EXPECT_EQ(t.Type, Antomic::TokenType::OpAssign);
    EXPECT_EQ(t.Line, 6);
    EXPECT_EQ(t.Column, 2);

    t = lex.Next();
    EXPECT_EQ(t.Type, Antomic::TokenType::String);
    EXPECT_EQ(t.Value, "another string");
    EXPECT_EQ(t.Line, 6);
    EXPECT_EQ(t.Column, 3);

    t = lex.Next();
    EXPECT_EQ(t.Type, Antomic::TokenType::Identifier);
    EXPECT_EQ(t.Value, "print");
    EXPECT_EQ(t.Line, 8);
    EXPECT_EQ(t.Column, 1);

    t = lex.Next();
    EXPECT_EQ(t.Type, Antomic::TokenType::SymbolParentesesOpen);
    EXPECT_EQ(t.Line, 8);
    EXPECT_EQ(t.Column, 6);

    t = lex.Next();
    EXPECT_EQ(t.Type, Antomic::TokenType::Identifier);
    EXPECT_EQ(t.Value, "b");
    EXPECT_EQ(t.Line, 8);
    EXPECT_EQ(t.Column, 7);

    t = lex.Next();
    EXPECT_EQ(t.Type, Antomic::TokenType::SymbolParentesesClose);
    EXPECT_EQ(t.Line, 8);
    EXPECT_EQ(t.Column, 8);

    t = lex.Next();
    EXPECT_EQ(t.Type, Antomic::TokenType::KeywordFor);
    EXPECT_EQ(t.Line, 10);
    EXPECT_EQ(t.Column, 1);

    t = lex.Next();
    EXPECT_EQ(t.Type, Antomic::TokenType::Identifier);
    EXPECT_EQ(t.Value, "a");
    EXPECT_EQ(t.Line, 10);
    EXPECT_EQ(t.Column, 5);

    t = lex.Next();
    EXPECT_EQ(t.Type, Antomic::TokenType::KeywordIn);
    EXPECT_EQ(t.Line, 10);
    EXPECT_EQ(t.Column, 7);

    t = lex.Next();
    EXPECT_EQ(t.Type, Antomic::TokenType::Identifier);
    EXPECT_EQ(t.Value, "range");
    EXPECT_EQ(t.Line, 10);
    EXPECT_EQ(t.Column, 10);

    t = lex.Next();
    EXPECT_EQ(t.Type, Antomic::TokenType::SymbolParentesesOpen);
    EXPECT_EQ(t.Line, 10);
    EXPECT_EQ(t.Column, 15);

    t = lex.Next();
    EXPECT_EQ(t.Type, Antomic::TokenType::NumberInteger);
    EXPECT_EQ(t.Value, "1");
    EXPECT_EQ(t.Line, 10);
    EXPECT_EQ(t.Column, 16);

    t = lex.Next();
    EXPECT_EQ(t.Type, Antomic::TokenType::SymbolParentesesClose);
    EXPECT_EQ(t.Line, 10);
    EXPECT_EQ(t.Column, 17);

    t = lex.Next();
    EXPECT_EQ(t.Type, Antomic::TokenType::SymbolColon);
    EXPECT_EQ(t.Line, 10);
    EXPECT_EQ(t.Column, 18);

    t = lex.Next();
    EXPECT_EQ(t.Type, Antomic::TokenType::Identation);
    EXPECT_EQ(t.Value.size(), 4);
    EXPECT_EQ(t.Line, 11);
    EXPECT_EQ(t.Column, 1);

    t = lex.Next();
    EXPECT_EQ(t.Type, Antomic::TokenType::Identifier);
    EXPECT_EQ(t.Value, "b");
    EXPECT_EQ(t.Line, 11);
    EXPECT_EQ(t.Column, 5);

    t = lex.Next();
    EXPECT_EQ(t.Type, Antomic::TokenType::OpAssign);
    EXPECT_EQ(t.Line, 11);
    EXPECT_EQ(t.Column, 7);

    t = lex.Next();
    EXPECT_EQ(t.Type, Antomic::TokenType::Identifier);
    EXPECT_EQ(t.Value, "a");
    EXPECT_EQ(t.Line, 11);
    EXPECT_EQ(t.Column, 9);

    t = lex.Next();
    EXPECT_EQ(t.Type, Antomic::TokenType::Identifier);
    EXPECT_EQ(t.Value, "c");
    EXPECT_EQ(t.Line, 13);
    EXPECT_EQ(t.Column, 1);

    t = lex.Next();
    EXPECT_EQ(t.Type, Antomic::TokenType::OpAssign);
    EXPECT_EQ(t.Line, 13);
    EXPECT_EQ(t.Column, 3);

    t = lex.Next();
    EXPECT_EQ(t.Type, Antomic::TokenType::NumberFloat);
    EXPECT_EQ(t.Value, "12.0");
    EXPECT_EQ(t.Line, 13);
    EXPECT_EQ(t.Column, 5);

    t = lex.Next();
    EXPECT_EQ(t.Type, Antomic::TokenType::OpAdd);
    EXPECT_EQ(t.Line, 13);
    EXPECT_EQ(t.Column, 10);

    t = lex.Next();
    EXPECT_EQ(t.Type, Antomic::TokenType::NumberHex);
    EXPECT_EQ(t.Value, "0x32");
    EXPECT_EQ(t.Line, 14);
    EXPECT_EQ(t.Column, 5);
}