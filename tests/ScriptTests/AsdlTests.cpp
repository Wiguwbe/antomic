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
#include <iostream>
#include "Script/Parser/Ast.h"

using namespace Antomic;

TEST(AntomicCoreTest, AsdlTests)
{
    auto expr = Expression(
        BinOp(
            Constant(1, "Int", 1, 1),
            operator_t::kAdd,
            Constant(1, "Int", 1, 3),
            1, 2));

    std::cout << to_string(expr) << std::endl;
}