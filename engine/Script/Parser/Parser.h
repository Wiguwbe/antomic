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
#pragma once
#include "Core/Base.h"
#include "Script/Parser/Lexer.h"
#include "Script/Parser/Ast.h"

namespace Antomic
{
    struct ParserState
    {
        std::stack<uint8_t> IdentationStack;
        std::stack<If_t> IfStack;
        std::stack<Try_t> TryStack;

        ParserState() {}
    };

    struct IdentifierTupleInfo
    {
        identifier Identifier;
        uint32_t Line;
        uint32_t Column;

        IdentifierTupleInfo() : Identifier(""), Line(0), Column(0) {}
        IdentifierTupleInfo(const identifier &i, uint32_t l, uint32_t c) : Identifier(i), Line(l), Column(c) {}
    };

    class Parser
    {
    public:
        Parser() = default;
        ~Parser() = default;
        mod_t FromFile(const std::string &name);
        mod_t FromExpression(const std::string &name);

    private:
        stmt_t TryStatement();
        stmt_t TryFunctionDef();
        stmt_t TryClassDef();
        stmt_t TryReturn();
        stmt_t TryDelete();
        expr_t TryDeleteExpr();
        stmt_t TryAssign(expr_t target);
        stmt_t TryAugAssign(expr_t target);
        stmt_t TryFor();
        expr_t TryForTarget();

        stmt_t TryWhile();
        expr_t TryWhileTest();
        stmt_t TryIfElse();
        stmt_t TryRaise();
        stmt_t TryTryExceptFinaly();
        stmt_t TryAssert();
        stmt_t TryImport();
        stmt_t TryImportFrom();
        stmt_t TryExpr();
        stmt_t TryPass();
        stmt_t TryBreak();
        stmt_t TryContinue();

        arguments_t TryArguments();
        baseclasses_t TryBaseClasses();
        baseclass_t TryBaseClass();

        expr_t TryExpression();
        expr_t TryExpressionOperand();
        expr_t TryExpressionOperator(expr_t left = nullptr);
        expr_t TryBoolOp(Token t, expr_t left, expr_t right);
        expr_t TryBinOp(Token t, expr_t left, expr_t right);
        expr_t TryUnaryOp(Token t, expr_t operand);
        expr_t TryCompare(Token t, expr_t left, expr_t right);

        expr_t TryLambda();
        expr_t TryIfExp();
        expr_t TryDict();
        expr_t TryCall(expr_t func);
        void TryCallArgs(std::vector<expr_t> &args);
        expr_t TryCallArg();
        expr_t TryCallArgOperand();
        expr_t TryCallArgOperator(expr_t left = nullptr);
        expr_t TryFormattedValue();
        expr_t TryConstant();
        expr_t TryAttribute(expr_t value);
        expr_t TrySubscript(expr_t value);
        expr_t TryName();
        expr_t TryList();
        expr_t TryTuple(expr_t first);
        expr_t TryIndex();
        expr_t TrySlice(expr_t first);
        arg_t TryArg();

    private:
        inline Token ReadNextToken()
        {
            return mLexer->Read();
        }

        inline Token PeekNextToken()
        {
            return mLexer->Peek();
        }

        inline void PushIdentation(uint8_t i)
        {
            if (mState.IdentationStack.empty())
            {
                mState.IdentationStack.push(i);
                return;
            }

            if (i == mState.IdentationStack.top())
                return;

            mState.IdentationStack.push(i);
        }

        inline void PopIdentation()
        {
            if (mState.IdentationStack.empty())
            {
                return;
            }
            mState.IdentationStack.pop();
        }

        inline uint8_t CurrentIdentation()
        {
            if (mState.IdentationStack.empty())
            {
                return 0;
            }

            return mState.IdentationStack.top();
        }

        bool TryBody(const std::string &name, std::vector<stmt_t> &body, bool allowEmpty = false);

    private:
        Ref<Lexer> mLexer = nullptr;
        ParserState mState;
    };
}