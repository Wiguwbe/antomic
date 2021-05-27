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

// class EvalVisitor : public BaseVisitor {
//  public:
 
//   std::any visitBinOp(BinOp_t node) override {
//     int left = std::any_cast<int>(visit(node->left));
//     int right = std::any_cast<int>(visit(node->right));
//     switch (node->op) {
//         case operator_t::kAdd: return left + right;
//         case operator_t::kSub: return left - right;
//         case operator_t::kMul: return left * right;
//         case operator_t::kDiv: return left / right;
//     }
//   }
  
//   std::any visitNum(Num_t node) override {
//     return node->n;
//   }

//   std::any visitOperator(operator_t value) override {
//     // not reached
//     return 0;
//   }
// };

TEST(AntomicCoreTest, AstTests)
{
    // TODO: Write more tests
    auto expr = Expression(
        BinOp(
            Constant(1, "Int", 1, 1),
            operator_t::kAdd,
            Constant(1, "Int", 1, 3),
            1, 2));

    
}