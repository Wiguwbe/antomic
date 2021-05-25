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

    class Parser
    {
    public:
        Parser() = default;
        ~Parser() = default;
        mod_t ParseModule(const std::string &name);
        mod_t ParseExpression(const std::string &name);
        mod_t ParseFunctionType() { return nullptr; }

    private:
        stmt_t ParseStatement();
        stmt_t ParseFunctionDef();
        stmt_t ParseClassDef();
        stmt_t ParseReturn();
        stmt_t ParseDelete();
        stmt_t ParseAssign();
        stmt_t ParseAugAssign();
        stmt_t ParseFor();
        stmt_t ParseWhile();
        stmt_t ParseIf();
        stmt_t ParseWith();
        stmt_t ParseRaise();
        stmt_t ParseTry();
        stmt_t ParseAssert();
        stmt_t ParseImport();
        stmt_t ParseImportFrom();
        stmt_t ParseExpr();
        stmt_t ParsePass();
        stmt_t ParseBreak();
        stmt_t ParseContinue();

        arguments_t ParseArguments();

        void ParseBoolOp(){};
        void ParseNamedExpr(){};
        void ParseBinOp(){};
        void ParseUnaryOp(){};
        void ParseLambda(){};
        void ParseIfExp(){};
        void ParseDict(){};
        void ParseSet(){};
        void ParseCompare(){};
        void ParseCall(){};
        void ParseFormattedValue(){};
        void ParseJoinedStr(){};
        void ParseConstant(){};
        void ParseAttribute(){};
        void ParseSubscript(){};
        void ParseStarred(){};
        void ParseName(){};
        void ParseList(){};
        void ParseTuple(){};
        void ParseSlice(){};
        void ParseExpr_Context(){};
        void ParseBoolop(){};
        void ParseOperator(){};
        void ParseUnaryop(){};
        void ParseCmpop(){};
        void ParseExceptHandler(){};
        void ParseArg(){};
        void ParseKeyword(){};
        void ParseAlias(){};
        void ParseWithitem(){};

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
            mState.IdentationStack.pop();
        }

        inline uint8_t CurrentIdentation()
        {
            if ( mState.IdentationStack.empty() )
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