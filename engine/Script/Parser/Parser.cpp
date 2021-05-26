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

#define AssertToken(x, y)     \
    auto x = ReadNextToken(); \
    ANTOMIC_ASSERT(x.Value == y, "Wrong token");

#define PeekToken(x, y)                                                                            \
    x = PeekNextToken();                                                                           \
    if (x.Type != y)                                                                               \
    {                                                                                              \
        ANTOMIC_ERROR("Unexpected token on line {0}, column {1}: {2}", x.Line, x.Column, x.Value); \
        return nullptr;                                                                            \
    }

#define TryToken(x, y) \
    PeekToken(x, y);   \
    x = ReadNextToken();

#define TryIdentifier(x, y)             \
    TryToken(x, TokenType::Identifier); \
    auto y = x.Value;

#define TrySeq(x, y)    \
    auto x = y();       \
    if (!x)             \
    {                   \
        return nullptr; \
    }

#define PinPosition(x, y, z) \
    auto y = x.Line;         \
    auto z = x.Column;

namespace Antomic
{
    mod_t Parser::FromFile(const std::string &name)
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
                auto stmt = TryStatement();
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

    mod_t Parser::FromExpression(const std::string &expression)
    {
        auto reader = Reader::FromString(expression, "expression");
        mLexer = CreateRef<Lexer>(reader);

        auto expr = TryExpression();
        if (!expr)
        {
            return nullptr;
        }

        return Expression(expr);
    }

    stmt_t Parser::TryStatement()
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
                return TryFunctionDef();
            case TokenType::KeywordClass:
                return TryClassDef();
            case TokenType::KeywordReturn:
                return TryReturn();
            case TokenType::KeywordDel:
                return TryDelete();
            case TokenType::KeywordFor:
                return TryFor();
            case TokenType::KeywordWhile:
                return TryWhile();
            case TokenType::KeywordIf:
                return TryIf();
            case TokenType::KeywordWith:
                return TryWith();
            case TokenType::KeywordRaise:
                return TryRaise();
            case TokenType::KeywordTry:
                return TryTry();
            case TokenType::KeywordAssert:
                return TryAssert();
            case TokenType::KeywordImport:
                return TryImport();
            case TokenType::KeywordFrom:
                return TryImportFrom();
            case TokenType::KeywordPass:
                return TryPass();
            case TokenType::KeywordBreak:
                return TryBreak();
            case TokenType::KeywordContinue:
                return TryContinue();
            default:
                return TryExpr();
            }
        }

        return nullptr;
    }

    stmt_t Parser::TryFunctionDef()
    {
        AssertToken(t, "def");
        PinPosition(t, lineno, colno);
        TryToken(t, TokenType::Identifier);
        TryIdentifier(t, identifier);
        TryToken(t, TokenType::SymbolParentesesOpen);
        TrySeq(args, TryArguments);
        TryToken(t, TokenType::SymbolColon);

        std::vector<stmt_t> statements;
        for (;;)
        {
            auto t = PeekNextToken();

            switch (t.Type)
            {
            case TokenType::End:
                if (statements.empty())
                {
                    ANTOMIC_ERROR("Missing function '{0}' body, on line {0}, column {1}", identifier, lineno, colno);
                    return nullptr;
                }
                ReadNextToken();
                return FunctionDef(identifier, args, statements, lineno, colno);
            case TokenType::Identation:
                if (t.Value.size() < CurrentIdentation())
                {
                    PopIdentation();
                    return FunctionDef(identifier, args, statements, lineno, colno);
                }
                if (t.Value.size() > CurrentIdentation() && statements.empty())
                {
                    ReadNextToken();
                    PushIdentation((uint8_t)t.Value.size());
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
                // Next token column is less then current identation
                // most probably this function/class has ended
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

                auto stmt = TryStatement();
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

    stmt_t Parser::TryClassDef()
    {
        AssertToken(t, "class");
        PinPosition(t, lineno, colno);
        TryIdentifier(t, identifier);
        TryToken(t, TokenType::SymbolParentesesOpen);
        TrySeq(baseclasses, TryBaseClasses);
        TryToken(t, TokenType::SymbolColon);

        std::vector<stmt_t> statements;
        for (;;)
        {
            auto t = PeekNextToken();

            switch (t.Type)
            {
            case TokenType::End:
                if (statements.empty())
                {
                    ANTOMIC_ERROR("Missing function '{0}' body, on line {0}, column {1}", identifier, lineno, colno);
                    return nullptr;
                }
                ReadNextToken();
                return ClassDef(identifier, baseclasses, statements, lineno, colno);
            case TokenType::Identation:
                if (t.Value.size() < CurrentIdentation())
                {
                    PopIdentation();
                    return ClassDef(identifier, baseclasses, statements, lineno, colno);
                }
                if (t.Value.size() > CurrentIdentation() && statements.empty())
                {
                    ReadNextToken();
                    PushIdentation((uint8_t)t.Value.size());
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
                // Next token column is less then current identation
                // most probably this function/class has ended
                if (t.Column < CurrentIdentation())
                {
                    if (statements.empty())
                    {
                        ANTOMIC_ERROR("Missing function '{0}' body, on line {0}, column {1}", identifier, lineno, colno);
                        return nullptr;
                    }

                    PopIdentation();
                    return ClassDef(identifier, baseclasses, statements, lineno, colno);
                }

                auto stmt = TryStatement();
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

    stmt_t Parser::TryReturn()
    {
        AssertToken(t, "class");
        PinPosition(t, lineno, colno);

        return nullptr;
    }

    stmt_t Parser::TryDelete()
    {
        return nullptr;
    }

    stmt_t Parser::TryAssign()
    {
        return nullptr;
    }

    stmt_t Parser::TryAugAssign()
    {
        return nullptr;
    }

    stmt_t Parser::TryFor()
    {
        return nullptr;
    }

    stmt_t Parser::TryWhile()
    {
        return nullptr;
    }

    stmt_t Parser::TryIf()
    {
        return nullptr;
    }

    stmt_t Parser::TryWith()
    {
        return nullptr;
    }

    stmt_t Parser::TryRaise()
    {
        return nullptr;
    }

    stmt_t Parser::TryTry()
    {
        return nullptr;
    }

    stmt_t Parser::TryAssert()
    {
        return nullptr;
    }

    stmt_t Parser::TryImport()
    {
        return nullptr;
    }

    stmt_t Parser::TryImportFrom()
    {
        return nullptr;
    }

    stmt_t Parser::TryExpr()
    {
        auto expr = TryExpression();
        if (!expr)
        {
            return nullptr;
        }
        return Expr(expr, expr->lineno, expr->col_offset);
    }

    stmt_t Parser::TryPass()
    {
        return nullptr;
    }

    stmt_t Parser::TryBreak()
    {
        return nullptr;
    }

    stmt_t Parser::TryContinue()
    {
        return nullptr;
    }

    arguments_t Parser::TryArguments()
    {
        std::vector<arg_t> args;

        auto identifier = std::string("");
        auto type = std::string("");
        auto lineno = 0;
        auto colno = 0;
        for (;;)
        {
            auto t = ReadNextToken();
            if (PeekNextToken().Type == TokenType::End)
            {
                ANTOMIC_ERROR("Expected ')' on line {0}, column {1}: {2}", t.Line, t.Column, t.Value);
                return nullptr;
            }

            switch (t.Type)
            {
            case TokenType::SymbolParentesesClose:
                if (identifier.size() > 0)
                {
                    args.push_back(arg(identifier, type, lineno, colno));
                }
                return arguments(args);
            case TokenType::SymbolComma:
                if (identifier.size() == 0)
                {
                    ANTOMIC_ERROR("Expected indentifier on line {0}, column {1}", t.Line, t.Column);
                    return nullptr;
                }
                args.push_back(arg(identifier, type, lineno, colno));
                identifier = "";
                type = "";
                break;
            case TokenType::SymbolColon:
                if (identifier.size() == 0)
                {
                    ANTOMIC_ERROR("Expected indentifier on line {0}, column {1}", t.Line, t.Column);
                    return nullptr;
                }
                continue;
            case TokenType::Identifier:
                if (identifier.size() > 0)
                {
                    type = t.Value;
                    continue;
                }
                identifier = t.Value;
                lineno = t.Line;
                colno = t.Column;
                continue;
            default:
                ANTOMIC_ERROR("Unexpected token on line {0}, column {1}: {2}", t.Line, t.Column, t.Value);
                return nullptr;
            }
        }

        return arguments(args);
    }

    baseclasses_t Parser::TryBaseClasses()
    {
        std::vector<baseclass_t> bases;

        auto identifier = std::string("");
        for (;;)
        {
            auto t = ReadNextToken();
            if (PeekNextToken().Type == TokenType::End)
            {
                ANTOMIC_ERROR("Expected ')' on line {0}, column {1}: {2}", t.Line, t.Column, t.Value);
                return nullptr;
            }

            switch (t.Type)
            {
            case TokenType::SymbolParentesesClose:
                if (identifier.size() > 0)
                {
                    bases.push_back(baseclass(identifier));
                }
                return baseclasses(bases);
            case TokenType::SymbolComma:
                if (identifier.size() == 0)
                {
                    ANTOMIC_ERROR("Expected indentifier on line {0}, column {1}", t.Line, t.Column);
                    return nullptr;
                }
                bases.push_back(baseclass(identifier));
                identifier = "";
                break;
            case TokenType::SymbolColon:
                if (identifier.size() == 0)
                {
                    ANTOMIC_ERROR("Expected indentifier on line {0}, column {1}", t.Line, t.Column);
                    return nullptr;
                }
                continue;
            case TokenType::Identifier:
                ANTOMIC_ASSERT(identifier.size() > 0, "Empty identifier");
                identifier = t.Value;
                continue;
            default:
                ANTOMIC_ERROR("Unexpected token on line {0}, column {1}: {2}", t.Line, t.Column, t.Value);
                return nullptr;
            }
        }

        return baseclasses(bases);
    }

    expr_t Parser::TryExpression()
    {
        expr_t Left;
        expr_t Right;

        for (;;)
        {
            auto t = ReadNextToken();
            if (PeekNextToken().Type == TokenType::End)
            {
                ANTOMIC_ERROR("Expected ')' on line {0}, column {1}: {2}", t.Line, t.Column, t.Value);
                return nullptr;
            }

            switch (t.Type)
            {
            case TokenType::SymbolSemiColon:
                break;
            case TokenType::SymbolPeriod:
                break;
            case TokenType::SymbolComma:
                break;
            case TokenType::SymbolColon:
                break;
            case TokenType::SymbolBraceOpen:
                break;
            case TokenType::SymbolBraceClose:
                break;
            case TokenType::SymbolBracketOpen:
                break;
            case TokenType::SymbolBracketClose:
                break;
            case TokenType::SymbolParentesesOpen:
                break;
            case TokenType::SymbolParentesesClose:
                break;
            case TokenType::Identifier:
                break;
            default:
                ANTOMIC_ERROR("Unexpected token on line {0}, column {1}: {2}", t.Line, t.Column, t.Value);
                return nullptr;
            }
        }
        return nullptr;
    }
}