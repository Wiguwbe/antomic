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
#include "Script/Parser/Parser.h"
#include "Core/Log.h"

namespace Antomic
{
    mod_t Parser::ParseModule(const std::string &name)
    {
        mLexer = CreateRef<Lexer>(name);
        std::vector<stmt_t> statements;
        for (;;)
        {
            auto t = PeekNextToken();
            switch (t.Type)
            {
            case TokenType::End:
                ReadNextToken();
                return Module(statements);
            case TokenType::Invalid:
                ReadNextToken();
                ANTOMIC_ERROR("Parsing error on line {0}, column {1}: {2}", t.Line, t.Column, t.Value);
                return nullptr;
            case TokenType::Comment:
                ReadNextToken();
                continue;
            default:
                auto stmt = ParseStatement();
                if (stmt == nullptr)
                {
                    return nullptr;
                }
                statements.push_back(stmt);
                break;
            }
        }

        return nullptr;
    }

    mod_t Parser::ParseExpression(const std::string &expression)
    {
        auto reader = Reader::FromString(expression, "expression");
        mLexer = CreateRef<Lexer>(reader);

        return nullptr;
    }

    stmt_t Parser::ParseStatement()
    {
        for (;;)
        {
            auto t = PeekNextToken();
            switch (t.Type)
            {
            case TokenType::Identation:
                if (t.Value.size() != CurrentIdentation())
                {
                    ANTOMIC_ERROR("Wrong identation on line {0}, column {1}: {2}", t.Line, t.Column, t.Value);
                    return nullptr;
                }
                continue;
            case TokenType::KeywordDef:
                return ParseFunctionDef();
            case TokenType::KeywordReturn:
                return ParseReturn();
            case TokenType::KeywordDel:
                return ParseDelete();
            case TokenType::KeywordFor:
                return ParseFor();
            case TokenType::KeywordWhile:
                return ParseWhile();
            case TokenType::KeywordIf:
                return ParseIf();
            case TokenType::KeywordWith:
                return ParseWith();
            case TokenType::KeywordRaise:
                return ParseRaise();
            case TokenType::KeywordTry:
                return ParseTry();
            case TokenType::KeywordAssert:
                return ParseAssert();
            case TokenType::KeywordImport:
                return ParseImport();
            case TokenType::KeywordFrom:
                return ParseImportFrom();
            case TokenType::KeywordPass:
                return ParsePass();
            case TokenType::KeywordBreak:
                return ParseBreak();
            case TokenType::KeywordContinue:
                return ParseContinue();
            default:
                return ParseExpr();
            }
        }

        return nullptr;
    }

    stmt_t Parser::ParseFunctionDef()
    {
        auto t = ReadNextToken();
        auto lineno = t.Line;
        auto colno = t.Column;

        t = ReadNextToken();
        if (t.Type != TokenType::Identifier)
        {
            ANTOMIC_ERROR("Invalid identifier on line {0}, column {1}: {2}", t.Line, t.Column, t.Value);
            return nullptr;
        }
        auto identifier = t.Value;

        t = ReadNextToken();
        if (t.Type != TokenType::SymbolParentesesOpen)
        {
            ANTOMIC_ERROR("Expected '(' on line {0}, column {1}: {2}", t.Line, t.Column, t.Value);
            return nullptr;
        }

        auto args = ParseArguments();
        if (!args)
        {
            return nullptr;
        }

        t = ReadNextToken();
        if (t.Type != TokenType::SymbolColon)
        {
            ANTOMIC_ERROR("Expected ':' on line {0}, column {1}: {2}", t.Line, t.Column, t.Value);
            return nullptr;
        }

        t = PeekNextToken();
        if (t.Type != TokenType::Identation)
        {
            ANTOMIC_ERROR("Wrong indentation on line {0}, column {1}: {2}", t.Line, t.Column, t.Value);
            return nullptr;
        }

        PushIdentation((uint32_t)t.Value.size());

        std::vector<stmt_t> statements;
        for (;;)
        {
            auto t = PeekNextToken();

            // Next token column is less then current identation
            // most probably this function
            if (t.Column < CurrentIdentation())
            {
                if (statements.empty())
                {
                    ANTOMIC_ERROR("Missing function '{0}' body, on line {0}, column {1}", identifier, lineno, colno);
                    return nullptr;
                }

                PopIdentation();
                return FunctionDef(identifier, args, statements, lineno, colno);
            }

            switch (t.Type)
            {
            case TokenType::End:
                ReadNextToken();
                return FunctionDef(identifier, args, statements, lineno, colno);
            case TokenType::Identation:
                if (t.Value.size() < CurrentIdentation())
                {
                    PopIdentation();
                    return FunctionDef(identifier, args, statements, lineno, colno);
                }
                ANTOMIC_ERROR("Wrong identation on line {0}, column {1}: {2}", t.Line, t.Column, t.Value);
                return nullptr;
            case TokenType::Invalid:
                ReadNextToken();
                ANTOMIC_ERROR("Parsing error on line {0}, column {1}: {2}", t.Line, t.Column, t.Value);
                return nullptr;
            case TokenType::Comment:
                ReadNextToken();
                continue;
            default:
                auto stmt = ParseStatement();
                if (stmt == nullptr)
                {
                    return nullptr;
                }
                statements.push_back(stmt);
                break;
            }
        }

        return nullptr;
    }

    stmt_t Parser::ParseClassDef()
    {
        return nullptr;
    }

    stmt_t Parser::ParseReturn()
    {
        return nullptr;
    }

    stmt_t Parser::ParseDelete()
    {
        return nullptr;
    }

    stmt_t Parser::ParseAssign()
    {
        return nullptr;
    }

    stmt_t Parser::ParseAugAssign()
    {
        return nullptr;
    }

    stmt_t Parser::ParseFor()
    {
        return nullptr;
    }

    stmt_t Parser::ParseWhile()
    {
        return nullptr;
    }

    stmt_t Parser::ParseIf()
    {
        return nullptr;
    }

    stmt_t Parser::ParseWith()
    {
        return nullptr;
    }

    stmt_t Parser::ParseRaise()
    {
        return nullptr;
    }

    stmt_t Parser::ParseTry()
    {
        return nullptr;
    }

    stmt_t Parser::ParseAssert()
    {
        return nullptr;
    }

    stmt_t Parser::ParseImport()
    {
        return nullptr;
    }

    stmt_t Parser::ParseImportFrom()
    {
        return nullptr;
    }

    stmt_t Parser::ParseExpr()
    {
        return nullptr;
    }

    stmt_t Parser::ParsePass()
    {
        return nullptr;
    }

    stmt_t Parser::ParseBreak()
    {
        return nullptr;
    }

    stmt_t Parser::ParseContinue()
    {
        return nullptr;
    }

    arguments_t Parser::ParseArguments()
    {
        while (PeekNextToken().Type != TokenType::SymbolParentesesClose)
        {
            auto t = ReadNextToken();
            if (PeekNextToken().Type == TokenType::End)
            {
                ANTOMIC_ERROR("Expected ')' on line {0}, column {1}: {2}", t.Line, t.Column, t.Value);
                return nullptr;
            }
        }

        return nullptr;
    }

}