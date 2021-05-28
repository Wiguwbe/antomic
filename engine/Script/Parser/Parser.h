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
        expr_t TryForIter();
        stmt_t TryWhile();
        expr_t TryWhileTest();
        stmt_t TryIf();
        stmt_t TryRaise();
        stmt_t TryTry();
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

        expr_t TryExpression(bool callArgs = false);
        expr_t TryBoolOp(expr_t left, bool callArgs = false);
        expr_t TryBinOp(expr_t left, bool callArgs = false);

        expr_t TryUnaryOp();
        expr_t TryLambda();
        expr_t TryIfExp();
        expr_t TryDict();
        expr_t TrySet();
        expr_t TryCompare(expr_t left, bool callArgs = false);
        expr_t TryCall(expr_t func);
        expr_t TryCallArg();
        expr_t TryCallArgName();
        expr_t TryFormattedValue();
        expr_t TryConstant();
        expr_t TryAttribute(expr_t value);
        expr_t TrySubscript();
        expr_t TryStarred();
        expr_t TryName(bool callArgs = false);
        expr_t TryList();
        expr_t TryTupleNames(identifier first, uint32_t line, uint32_t column);
        expr_t TryTuple(expr_t first);
        expr_t TrySlice();

        excepthandler_t TryExceptHandler();
        arg_t TryArg();
        alias_t TryAlias();

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

    private:
        Ref<Lexer> mLexer = nullptr;
        ParserState mState;
    };
}