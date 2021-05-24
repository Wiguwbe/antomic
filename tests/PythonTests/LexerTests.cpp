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
    Antomic::Lexer lex("tests/files/test.py");

    Antomic::Token t = lex.Next();
    EXPECT_EQ(t.Type, Antomic::TokenType::Comment);
    EXPECT_EQ(t.Value, "# A commment");
    EXPECT_EQ(t.Line, 1);
    EXPECT_EQ(t.Column, 1);

    t = lex.Next();
    EXPECT_EQ(t.Type, Antomic::TokenType::NewLine);
    EXPECT_EQ(t.Line, 1);
    EXPECT_EQ(t.Column, 13);

    t = lex.Next();
    EXPECT_EQ(t.Type, Antomic::TokenType::Identifier);
    EXPECT_EQ(t.Value, "a");
    EXPECT_EQ(t.Line, 2);
    EXPECT_EQ(t.Column, 1);

    t = lex.Next();
    EXPECT_EQ(t.Type, Antomic::TokenType::Space);
    EXPECT_EQ(t.Line, 2);
    EXPECT_EQ(t.Column, 2);

    t = lex.Next();
    EXPECT_EQ(t.Type, Antomic::TokenType::OpAssign);
    EXPECT_EQ(t.Line, 2);
    EXPECT_EQ(t.Column, 3);

    t = lex.Next();
    EXPECT_EQ(t.Type, Antomic::TokenType::Space);
    EXPECT_EQ(t.Line, 2);
    EXPECT_EQ(t.Column, 4);

    t = lex.Next();
    EXPECT_EQ(t.Type, Antomic::TokenType::String);
    EXPECT_EQ(t.Value, "a string");
    EXPECT_EQ(t.Line, 2);
    EXPECT_EQ(t.Column, 5);

    t = lex.Next();
    EXPECT_EQ(t.Type, Antomic::TokenType::NewLine);
    EXPECT_EQ(t.Line, 2);
    EXPECT_EQ(t.Column, 15);

    t = lex.Next();
    EXPECT_EQ(t.Type, Antomic::TokenType::Identifier);
    EXPECT_EQ(t.Value, "print");
    EXPECT_EQ(t.Line, 3);
    EXPECT_EQ(t.Column, 1);

    t = lex.Next();
    EXPECT_EQ(t.Type, Antomic::TokenType::DelimParentesesOpen);
    EXPECT_EQ(t.Line, 3);
    EXPECT_EQ(t.Column, 6);

    t = lex.Next();
    EXPECT_EQ(t.Type, Antomic::TokenType::String);
    EXPECT_EQ(t.Value, " a multiline string \n    asd");
    EXPECT_EQ(t.Line, 3);
    EXPECT_EQ(t.Column, 7);

    t = lex.Next();
    EXPECT_EQ(t.Type, Antomic::TokenType::DelimParentesesClose);
    EXPECT_EQ(t.Line, 4);
    EXPECT_EQ(t.Column, 9);

    t = lex.Next();
    EXPECT_EQ(t.Type, Antomic::TokenType::NewLine);
    EXPECT_EQ(t.Line, 4);
    EXPECT_EQ(t.Column, 10);

    t = lex.Next();
    EXPECT_EQ(t.Type, Antomic::TokenType::NewLine);
    EXPECT_EQ(t.Line, 5);
    EXPECT_EQ(t.Column, 1);

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
    EXPECT_EQ(t.Type, Antomic::TokenType::NewLine);
    EXPECT_EQ(t.Line, 6);
    EXPECT_EQ(t.Column, 19);

    t = lex.Next();
    EXPECT_EQ(t.Type, Antomic::TokenType::NewLine);
    EXPECT_EQ(t.Line, 7);
    EXPECT_EQ(t.Column, 1);

    t = lex.Next();
    EXPECT_EQ(t.Type, Antomic::TokenType::Identifier);
    EXPECT_EQ(t.Value, "print");
    EXPECT_EQ(t.Line, 8);
    EXPECT_EQ(t.Column, 1);

    t = lex.Next();
    EXPECT_EQ(t.Type, Antomic::TokenType::DelimParentesesOpen);
    EXPECT_EQ(t.Line, 8);
    EXPECT_EQ(t.Column, 6);

    t = lex.Next();
    EXPECT_EQ(t.Type, Antomic::TokenType::Identifier);
    EXPECT_EQ(t.Value, "b");
    EXPECT_EQ(t.Line, 8);
    EXPECT_EQ(t.Column, 7);

    t = lex.Next();
    EXPECT_EQ(t.Type, Antomic::TokenType::DelimParentesesClose);
    EXPECT_EQ(t.Line, 8);
    EXPECT_EQ(t.Column, 8);

    t = lex.Next();
    EXPECT_EQ(t.Type, Antomic::TokenType::NewLine);
    EXPECT_EQ(t.Line, 8);
    EXPECT_EQ(t.Column, 9);

    t = lex.Next();
    EXPECT_EQ(t.Type, Antomic::TokenType::NewLine);
    EXPECT_EQ(t.Line, 9);
    EXPECT_EQ(t.Column, 1);

    t = lex.Next();
    EXPECT_EQ(t.Type, Antomic::TokenType::KeywordFor);
    EXPECT_EQ(t.Line, 10);
    EXPECT_EQ(t.Column, 1);

    // Whitespace
    t = lex.Next();

    t = lex.Next();
    EXPECT_EQ(t.Type, Antomic::TokenType::Identifier);
    EXPECT_EQ(t.Value, "a");
    EXPECT_EQ(t.Line, 10);
    EXPECT_EQ(t.Column, 5);

    // Whitespace
    t = lex.Next();

    t = lex.Next();
    EXPECT_EQ(t.Type, Antomic::TokenType::KeywordIn);
    EXPECT_EQ(t.Line, 10);
    EXPECT_EQ(t.Column, 7);

    // Whitespace
    t = lex.Next();

    t = lex.Next();
    EXPECT_EQ(t.Type, Antomic::TokenType::Identifier);
    EXPECT_EQ(t.Value, "range");
    EXPECT_EQ(t.Line, 10);
    EXPECT_EQ(t.Column, 10);

    // t = lex.Next();
    // EXPECT_EQ(t.Type, Antomic::TokenType::NewLine);
    // EXPECT_EQ(t.Line, 2);
    // EXPECT_EQ(t.Column, 11);

    // t = lex.Next();
    // EXPECT_EQ(t.Type, Antomic::TokenType::String);
    // EXPECT_EQ(t.Value, " a multiline string \n    asd");
    // EXPECT_EQ(t.Line, 3);
    // EXPECT_EQ(t.Column, 1);

    // t = lex.Next();
    // EXPECT_EQ(t.Type, Antomic::TokenType::NewLine);
    // EXPECT_EQ(t.Line, 4);
    // EXPECT_EQ(t.Column, 32);

    // t = lex.Next();
    // EXPECT_EQ(t.Type, Antomic::TokenType::String);
    // EXPECT_EQ(t.Value, "another string");
    // EXPECT_EQ(t.Line, 5);
    // EXPECT_EQ(t.Column, 1);
}