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
#include "Script/Parser/Parser.h"

TEST(AntomicCoreTest, ParserTests)
{
    // std::string expression1 = "b+(a+c+(a+b))";
    // std::string expression2 = "a and b and ( d or c )";
    Antomic::Parser parser;
    // auto expr1 = parser.FromExpression(expression1);
    // auto expr2 = parser.FromExpression(expression2);

    // if (expr1)
    // {
    //     std::cout << Antomic::to_string(expr1) << std::endl;
    // }
    // if (expr2)
    // {
    //     std::cout << Antomic::to_string(expr2) << std::endl;
    // }

    auto mod1 = parser.FromFile("tests/files/parser_test.py");

    if (mod1)
    {
        std::cout << Antomic::to_string(mod1) << std::endl;
    }

}