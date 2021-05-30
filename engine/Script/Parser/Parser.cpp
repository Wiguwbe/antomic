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

#define AssertTokenValue(x, y) \
    x = ReadNextToken();       \
    ANTOMIC_ASSERT(x.Value == y, "Wrong token");

#define AssertTokenType(x, y) \
    x = ReadNextToken();      \
    ANTOMIC_ASSERT(x.Type == y, "Wrong token");

#define EndOrNewLine(x)                                                                            \
    x = PeekNextToken();                                                                           \
    if (x.Type != TokenType::End && x.Type != TokenType::NewLine)                                  \
    {                                                                                              \
        ANTOMIC_ERROR("Unexpected token on line {0}, column {1}: {2}", x.Line, x.Column, x.Value); \
        return nullptr;                                                                            \
    }                                                                                              \
    x = ReadNextToken();

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

#define TryParsing(x, y, z, ...) \
    x = z(__VA_ARGS__);          \
    if (!x)                      \
    {                            \
        return y;                \
    }

#define TryParsingAuto(x, y, z, ...) \
    auto x = z(__VA_ARGS__);         \
    if (!x)                          \
    {                                \
        return y;                    \
    }

#define PinPosition(x, y, z) \
    auto y = x.Line;         \
    auto z = x.Column;

#define ParsingError(x, y)                                                                    \
    ANTOMIC_ERROR("Parsing error on line {0}, column {1}: '{2}'", x.Line, x.Column, x.Value); \
    return y;

#define WrongIdentation(x, y)                                                                               \
    ANTOMIC_ERROR("Wrong identation on line {0}, column {1}: got '{2}'", x.Line, x.Column, x.Value.size()); \
    return y;

#define MissingBody(x, y, z)                                                              \
    ANTOMIC_ERROR("Missing body for '{0}' on line {1}, column {2}", y, x.Line, x.Column); \
    return z;

#define ExpectedToken(x, y, z)                                                                                        \
    ANTOMIC_ERROR("Unexpected token on line {0}, column {1}: got '{2}', expected {3}", x.Line, x.Column, x.Value, y); \
    return z;

#define UnexpectedToken(x, y)                                                                    \
    ANTOMIC_ERROR("Unexpected token on line {0}, column {1}: '{2}'", t.Line, t.Column, t.Value); \
    return y;

#define MustNotHave(x, y, z) \
    if (x)                   \
    {                        \
        ParsingError(y, z);  \
    }

#define MustHave(x, y, z)   \
    if (!x)                 \
    {                       \
        ParsingError(y, z); \
    }

#define UnsupportedCallArgs(x, y) \
    if (noAssign)                 \
    {                             \
        UnexpectedToken(x, y);    \
    }

#define IgnoreCallArgs(x) \
    if (callArgs)         \
    {                     \
        return x;         \
    }

#define HaveBody(x, y, ...)          \
    if (!TryBody(x, y, __VA_ARGS__)) \
    {                                \
        return nullptr;              \
    }

#define EmptyStmtBody \
    std::vector<stmt_t> {}

#define EmptyHandlers \
    std::vector<excepthandler_t> {}

#define NewStatement(x, y, z, ...) auto x = dynamic_cast<y>(z(__VA_ARGS__));

#define ClearStack(x)  \
    while (!x.empty()) \
    {                  \
        x.pop();       \
    }

namespace Antomic
{
    mod_t Parser::FromFile(const std::string &name)
    {
        mLexer = CreateRef<Lexer>(name);
        std::vector<stmt_t> body;
        NewStatement(mod, Module_t, Module, EmptyStmtBody);
        HaveBody(name, mod->body);
        ClearStack(mState.IfStack);
        ClearStack(mState.TryStack);
        return mod;
    }

    mod_t Parser::FromExpression(const std::string &expression)
    {
        auto reader = Reader::FromString(expression, "expression");
        mLexer = CreateRef<Lexer>(reader);
        TryParsingAuto(expr, nullptr, TryExpression, false, false, false);
        ClearStack(mState.IfStack);
        ClearStack(mState.TryStack);
        return Expression(expr);
    }

    bool Parser::TryBody(const std::string &name, std::vector<stmt_t> &body, bool allowEmpty)
    {
        for (;;)
        {
            auto t = PeekNextToken();
            switch (t.Type)
            {
            case TokenType::End:
                if (body.empty() && !allowEmpty)
                {
                    MissingBody(t, name, false);
                }
                return true;
            case TokenType::Identation:
                if (t.Value.size() < CurrentIdentation())
                {
                    if (body.empty() && !allowEmpty)
                    {
                        MissingBody(t, name, false);
                    }

                    PopIdentation();
                    return true;
                }
                if (t.Value.size() > CurrentIdentation() && !body.empty())
                {
                    WrongIdentation(t, false);
                }
                ReadNextToken();
                PushIdentation((uint8_t)t.Value.size());
                continue;
            case TokenType::Invalid:
                ParsingError(t, false);
            case TokenType::NewLine:
            case TokenType::Comment:
                ReadNextToken();
                continue;
            case TokenType::KeywordElse:
            case TokenType::KeywordElIf:
            {
                MustNotHave(mState.IfStack.empty(), t, false);
                TryParsingAuto(stmt, false, TryStatement);
                continue;
            }
            case TokenType::KeywordExcept:
            case TokenType::KeywordFinally:
            {
                MustNotHave(mState.TryStack.empty(), t, false);
                TryParsingAuto(stmt, false, TryStatement);
                continue;
            }
            default:
            {
                TryParsingAuto(stmt, false, TryStatement);
                body.push_back(stmt);
                break;
            }
            }
        }
        return false;
    }

    stmt_t Parser::TryStatement()
    {
        for (;;)
        {
            auto t = PeekNextToken();
            switch (t.Type)
            {
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
            case TokenType::KeywordElse:
            case TokenType::KeywordElIf:
                return TryIfElse();
            case TokenType::KeywordRaise:
                return TryRaise();
            case TokenType::KeywordTry:
            case TokenType::KeywordExcept:
            case TokenType::KeywordFinally:
                return TryTryExceptFinaly();
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
        Token t;
        TryToken(t, TokenType::KeywordDef);
        PinPosition(t, lineno, colno);
        TryIdentifier(t, identifier);
        TryToken(t, TokenType::SymbolParentesesOpen);
        TryParsingAuto(args, nullptr, TryArguments);
        TryToken(t, TokenType::SymbolColon);
        TryToken(t, TokenType::NewLine);
        NewStatement(stmt, FunctionDef_t, FunctionDef, identifier, args, EmptyStmtBody, lineno, colno);
        HaveBody(identifier, stmt->body);
        return stmt;
    }

    stmt_t Parser::TryClassDef()
    {
        Token t;
        TryToken(t, TokenType::KeywordClass);
        PinPosition(t, lineno, colno);
        TryIdentifier(t, identifier);
        TryToken(t, TokenType::SymbolParentesesOpen);
        TryParsingAuto(baseclasses, nullptr, TryBaseClasses);
        TryToken(t, TokenType::SymbolColon);
        TryToken(t, TokenType::NewLine);
        NewStatement(stmt, ClassDef_t, ClassDef, identifier, baseclasses, EmptyStmtBody, lineno, colno);
        HaveBody(identifier, stmt->body);
        return stmt;
    }

    stmt_t Parser::TryReturn()
    {
        Token t;
        expr_t value = nullptr;
        TryToken(t, TokenType::KeywordReturn);
        t = PeekNextToken();
        if (t.Type != TokenType::NewLine && t.Type != TokenType::End)
        {
            TryParsing(value, nullptr, TryExpression, true, false, false);
        }
        EndOrNewLine(t);
        return Return(value, t.Line, t.Column);
    }

    stmt_t Parser::TryDelete()
    {
        Token t;
        TryToken(t, TokenType::KeywordDel);
        TryParsingAuto(expr, nullptr, TryDeleteExpr);
        EndOrNewLine(t);
        return Delete(expr, t.Line, t.Column);
    }

    expr_t Parser::TryDeleteExpr()
    {
        Token t;
        TryIdentifier(t, identifier);
        return Name(identifier, expr_context_t::kDel, t.Line, t.Column);
    }

    stmt_t Parser::TryAssign(expr_t target)
    {
        Token t;
        TryToken(t, TokenType::OpAssign);
        TryParsingAuto(expr, nullptr, TryExpression, false, false, false);
        EndOrNewLine(t);
        return Assign(target, expr, t.Line, t.Column);
    }

    stmt_t Parser::TryAugAssign(expr_t target)
    {
        Token t = ReadNextToken();
        TryParsingAuto(expr, nullptr, TryExpression, false, false, false);
        EndOrNewLine(t);

        switch (t.Type)
        {
        case TokenType::OpAddAssign:
            return AugAssign(target, operator_t::kAdd, expr, t.Line, t.Column);
        case TokenType::OpSubAssign:
            return AugAssign(target, operator_t::kSub, expr, t.Line, t.Column);
        case TokenType::OpMulAssign:
            return AugAssign(target, operator_t::kMult, expr, t.Line, t.Column);
        case TokenType::OpDivAssign:
            return AugAssign(target, operator_t::kDiv, expr, t.Line, t.Column);
        case TokenType::OpFloorDivAssign:
            return AugAssign(target, operator_t::kFloorDiv, expr, t.Line, t.Column);
        case TokenType::OpModAssign:
            return AugAssign(target, operator_t::kMod, expr, t.Line, t.Column);
        case TokenType::OpAndAssign:
            return AugAssign(target, operator_t::kBitAnd, expr, t.Line, t.Column);
        case TokenType::OpOrAssign:
            return AugAssign(target, operator_t::kBitOr, expr, t.Line, t.Column);
        case TokenType::OpXorAssign:
            return AugAssign(target, operator_t::kBitXor, expr, t.Line, t.Column);
        case TokenType::OpShiftLeftAssign:
            return AugAssign(target, operator_t::kLShift, expr, t.Line, t.Column);
        case TokenType::OpShiftRightAssign:
            return AugAssign(target, operator_t::kRShift, expr, t.Line, t.Column);
        case TokenType::OpExpAssign:
            return AugAssign(target, operator_t::kPow, expr, t.Line, t.Column);
        default:
            UnexpectedToken(t, nullptr);
        }

        return nullptr;
    }

    stmt_t Parser::TryFor()
    {
        Token t;
        TryToken(t, TokenType::KeywordFor);
        PinPosition(t, lineno, colno);
        TryParsingAuto(target, nullptr, TryForTarget);
        TryToken(t, TokenType::KeywordIn);
        TryParsingAuto(iter, nullptr, TryExpression, true, false, true);
        TryToken(t, TokenType::SymbolColon);
        NewStatement(stmt, For_t, For, target, iter, EmptyStmtBody, lineno, colno);
        t = PeekNextToken();
        switch (t.Type)
        {
        case TokenType::NewLine:
        {
            ReadNextToken();
            HaveBody("for", stmt->body);
            break;
        }
        default:
        {
            TryParsingAuto(body, nullptr, TryStatement);
            stmt->body.push_back(body);
            break;
        }
        }
        return stmt;
    }

    expr_t Parser::TryForTarget()
    {
        std::vector<expr_t> names;
        for (;;)
        {
            auto t = PeekNextToken();
            switch (t.Type)
            {
            case TokenType::End:
                ExpectedToken(t, "in", nullptr);
            case TokenType::KeywordIn:
                if (names.size() == 0)
                {
                    ParsingError(t, nullptr);
                }
                return names.size() == 1 ? names.front() : Tuple(names, expr_context_t::kStore, t.Line, t.Column);
            case TokenType::SymbolComma:
                ReadNextToken();
                continue;
            case TokenType::Identifier:
            {
                TryIdentifier(t, identifier);
                names.push_back(Name(identifier, expr_context_t::kStore, t.Line, t.Column));
                continue;
            }
            default:
                ParsingError(t, nullptr);
            }
        }

        return nullptr;
    }

    stmt_t Parser::TryWhile()
    {
        Token t;
        TryToken(t, TokenType::KeywordWhile);
        PinPosition(t, lineno, colno);
        TryToken(t, TokenType::SymbolParentesesOpen);
        TryParsingAuto(test, nullptr, TryExpression, true, false, true);
        TryToken(t, TokenType::SymbolParentesesClose);
        TryToken(t, TokenType::SymbolColon);
        NewStatement(stmt, While_t, While, test, EmptyStmtBody, lineno, colno);
        t = PeekNextToken();
        switch (t.Type)
        {
        case TokenType::NewLine:
        {
            ReadNextToken();
            HaveBody("while", stmt->body);
        }
        default:
        {
            TryParsingAuto(body, nullptr, TryStatement);
            stmt->body.push_back(stmt);
            break;
        }
        }
        return stmt;
    }

    stmt_t Parser::TryIfElse()
    {
        Token t = ReadNextToken();
        PinPosition(t, lineno, colno);
        switch (t.Type)
        {
        case TokenType::KeywordIf:
        {
            TryParsingAuto(test, nullptr, TryExpression, true, false, true);
            TryToken(t, TokenType::SymbolColon);
            NewStatement(stmt, If_t, If, test, EmptyStmtBody, EmptyStmtBody, lineno, colno);
            t = PeekNextToken();
            switch (t.Type)
            {
            case TokenType::NewLine:
                ReadNextToken();
                HaveBody("if", stmt->body);
                mState.IfStack.push(stmt);
                break;
            default:
                TryParsingAuto(body, nullptr, TryStatement);
                stmt->body.push_back(body);
                break;
            }
            return stmt;
        }
        case TokenType::KeywordElse:
        {
            MustNotHave(mState.IfStack.empty(), t, nullptr);
            TryToken(t, TokenType::SymbolColon);
            t = PeekNextToken();
            auto stmt = dynamic_cast<If_t>(mState.IfStack.top());
            switch (t.Type)
            {
            case TokenType::NewLine:
                ReadNextToken();
                HaveBody("else", stmt->orelse);
                break;
            default:
                TryParsingAuto(body, nullptr, TryStatement);
                stmt->orelse.push_back(stmt);
                break;
            }
            return stmt;
        }
        case TokenType::KeywordElIf:
        {
            MustNotHave(mState.IfStack.empty(), t, nullptr);
            TryParsingAuto(test, nullptr, TryExpression, true, false, true);
            TryToken(t, TokenType::SymbolColon);
            NewStatement(orelse, If_t, If, test, EmptyStmtBody, EmptyStmtBody, lineno, colno);
            auto stmt = dynamic_cast<If_t>(mState.IfStack.top());
            mState.IfStack.pop();
            mState.IfStack.push(orelse);
            stmt->orelse.push_back(orelse);
            t = PeekNextToken();
            switch (t.Type)
            {
            case TokenType::NewLine:
                ReadNextToken();
                HaveBody("elif", orelse->body);
                break;
            default:
                TryParsingAuto(body, nullptr, TryStatement);
                orelse->body.push_back(body);
                break;
            }
            return orelse;
        }
        }
        UnexpectedToken(t, nullptr);
    }

    stmt_t Parser::TryRaise()
    {
        Token t;
        TryToken(t, TokenType::KeywordRaise);
        TryParsingAuto(expr, nullptr, TryExpression, true, false, true);
        EndOrNewLine(t);
        return Raise(expr, t.Line, t.Column);
    }

    stmt_t Parser::TryTryExceptFinaly()
    {
        Token t = ReadNextToken();
        PinPosition(t, lineno, colno);
        switch (t.Type)
        {
        case TokenType::KeywordTry:
        {
            Token t;
            TryToken(t, TokenType::SymbolColon);
            EndOrNewLine(t);
            NewStatement(stmt, Try_t, Try, EmptyStmtBody, EmptyHandlers, EmptyStmtBody, lineno, colno);
            mState.TryStack.push(stmt);
            HaveBody("try", stmt->body);
            return stmt;
        }
        case TokenType::KeywordExcept:
        {
            Token t;
            MustNotHave(mState.TryStack.empty(), t, nullptr);
            TryIdentifier(t, identifier);
            auto type = Name(identifier, expr_context_t::kLoad, t.Line, t.Column);
            TryToken(t, TokenType::KeywordAs);
            TryIdentifier(t, name);
            TryToken(t, TokenType::SymbolColon);
            EndOrNewLine(t);
            auto stmt = dynamic_cast<Try_t>(mState.TryStack.top());
            NewStatement(handler, ExceptHandler_t, ExceptHandler, type, name, EmptyStmtBody, lineno, colno);
            HaveBody("except", handler->body);
            stmt->handlers.push_back(handler);
            return stmt;
        }
        case TokenType::KeywordFinally:
        {
            MustNotHave(mState.TryStack.empty(), t, nullptr);
            Token t;
            TryToken(t, TokenType::SymbolColon);
            EndOrNewLine(t);
            auto stmt = dynamic_cast<Try_t>(mState.TryStack.top());
            HaveBody("finally", stmt->finalbody);
            return stmt;
        }
        }
        UnexpectedToken(t, nullptr);
    }

    stmt_t Parser::TryAssert()
    {
        Token t;
        expr_t msg = nullptr;
        TryToken(t, TokenType::KeywordAssert);
        PinPosition(t, lineno, colno);
        TryToken(t, TokenType::SymbolParentesesOpen);
        TryParsingAuto(test, nullptr, TryExpression, true, true, false);
        t = PeekNextToken();
        if (t.Type == TokenType::SymbolComma)
        {
            ReadNextToken();
            TryParsing(msg, nullptr, TryExpression, true, true, false);
        }
        TryToken(t, TokenType::SymbolParentesesClose);
        EndOrNewLine(t);
        return Assert(test, msg, lineno, colno);
    }

    stmt_t Parser::TryImport()
    {
        Token t;
        std::vector<alias_t> names;
        identifier name = "";
        identifier asName = "";
        TryToken(t, TokenType::KeywordImport);
        PinPosition(t, lineno, colno);
        for (;;)
        {
            auto t = PeekNextToken();
            switch (t.Type)
            {
            case TokenType::NewLine:
            case TokenType::End:
                ReadNextToken();
                if (name.size() != 0)
                {
                    names.push_back(alias(name, asName, t.Line, t.Column));
                }
                return Import(names, lineno, colno);
            case TokenType::SymbolComma:
                ReadNextToken();
                names.push_back(alias(name, asName, t.Line, t.Column));
                name = "";
                asName = "";
                continue;
            case TokenType::KeywordAs:
            {
                ReadNextToken();
                TryIdentifier(t, identifier);
                asName = identifier;
                continue;
            }
            case TokenType::Identifier:
            {
                TryIdentifier(t, identifier);
                name = identifier;
                continue;
            }
            default:
                ParsingError(t, nullptr);
            }
        }
        return nullptr;
    }

    stmt_t Parser::TryImportFrom()
    {
        Token t;
        std::vector<alias_t> names;
        identifier name = "";
        identifier asName = "";
        TryToken(t, TokenType::KeywordFrom);
        PinPosition(t, lineno, colno);
        TryIdentifier(t, identifier);
        TryToken(t, TokenType::KeywordImport);
        for (;;)
        {
            auto t = PeekNextToken();
            switch (t.Type)
            {
            case TokenType::NewLine:
            case TokenType::End:
                ReadNextToken();
                if (name.size() != 0)
                {
                    names.push_back(alias(name, asName, t.Line, t.Column));
                }
                return ImportFrom(identifier, names, lineno, colno);
            case TokenType::SymbolComma:
                ReadNextToken();
                names.push_back(alias(name, asName, t.Line, t.Column));
                name = "";
                asName = "";
                continue;
            case TokenType::KeywordAs:
            {
                ReadNextToken();
                TryIdentifier(t, identifier);
                asName = identifier;
                continue;
            }
            case TokenType::Identifier:
            {
                TryIdentifier(t, identifier);
                name = identifier;
                continue;
            }
            default:
                ParsingError(t, nullptr);
            }
        }
        return nullptr;
    }

    stmt_t Parser::TryExpr()
    {
        expr_t expr = nullptr;
        for (;;)
        {
            auto t = PeekNextToken();
            switch (t.Type)
            {
            case TokenType::NewLine:
                ReadNextToken();
            case TokenType::End:
                MustHave(expr, t, nullptr);
                return Expr(expr, expr->lineno, expr->col_offset);
            case TokenType::OpAssign:
                MustHave(expr, t, nullptr);
                return TryAssign(expr);
            case TokenType::OpAddAssign:
            case TokenType::OpSubAssign:
            case TokenType::OpMulAssign:
            case TokenType::OpDivAssign:
            case TokenType::OpFloorDivAssign:
            case TokenType::OpModAssign:
            case TokenType::OpAndAssign:
            case TokenType::OpOrAssign:
            case TokenType::OpXorAssign:
            case TokenType::OpShiftLeftAssign:
            case TokenType::OpShiftRightAssign:
            case TokenType::OpExpAssign:
                MustHave(expr, t, nullptr);
                return TryAugAssign(expr);
            default:
                TryParsing(expr, nullptr, TryExpression, false, false, false);
            }
        }
        return nullptr;
    }

    stmt_t Parser::TryPass()
    {
        Token t;
        TryToken(t, TokenType::KeywordPass);
        EndOrNewLine(t);
        return Pass(t.Line, t.Column);
    }

    stmt_t Parser::TryBreak()
    {
        Token t;
        TryToken(t, TokenType::KeywordBreak);
        EndOrNewLine(t);
        return Break(t.Line, t.Column);
    }

    stmt_t Parser::TryContinue()
    {
        Token t;
        TryToken(t, TokenType::KeywordContinue);
        EndOrNewLine(t);
        return Continue(t.Line, t.Column);
    }

    arguments_t Parser::TryArguments()
    {
        std::vector<arg_t> args;

        for (;;)
        {
            auto t = PeekNextToken();

            switch (t.Type)
            {
            case TokenType::End:
            case TokenType::NewLine:
                ExpectedToken(t, ')', nullptr);
            case TokenType::SymbolParentesesClose:
                ReadNextToken();
                return arguments(args);
            case TokenType::SymbolComma:
                ReadNextToken();
                continue;
            default:
                TryParsingAuto(arg, nullptr, TryArg);
                args.push_back(arg);
                break;
            }
        }

        return arguments(args);
    }

    arg_t Parser::TryArg()
    {
        Token t;
        TryIdentifier(t, identifier);
        PinPosition(t, lineno, colno);

        t = PeekNextToken();
        if (t.Type == TokenType::SymbolColon)
        {
            ReadNextToken();
            TryIdentifier(t, type);
            return arg(identifier, type, lineno, colno);
        }
        return arg(identifier, "object", lineno, colno);
    }

    baseclasses_t Parser::TryBaseClasses()
    {
        std::vector<baseclass_t> bases;

        for (;;)
        {
            auto t = PeekNextToken();

            switch (t.Type)
            {
            case TokenType::End:
            case TokenType::NewLine:
            case TokenType::SymbolColon:
                ExpectedToken(t, ')', nullptr);
            case TokenType::SymbolParentesesClose:
                ReadNextToken();
                return baseclasses(bases);
            case TokenType::SymbolComma:
                continue;
            default:
                TryParsingAuto(base, nullptr, TryBaseClass);
                bases.push_back(base);
                break;
            }
        }

        return baseclasses(bases);
    }

    baseclass_t Parser::TryBaseClass()
    {
        Token t;
        PinPosition(t, lineno, colno);
        TryIdentifier(t, identifier);
        return baseclass(identifier, lineno, colno);
    }

    expr_t Parser::TryExpression(bool noAssign, bool callArgs, bool allowColon)
    {
        expr_t expr = nullptr;
        for (;;)
        {
            auto t = PeekNextToken();
            switch (t.Type)
            {
            case TokenType::SymbolParentesesClose:
            case TokenType::End:
            case TokenType::NewLine:
                return expr;
            case TokenType::OpAssign:
            case TokenType::OpAddAssign:
            case TokenType::OpSubAssign:
            case TokenType::OpMulAssign:
            case TokenType::OpDivAssign:
            case TokenType::OpFloorDivAssign:
            case TokenType::OpModAssign:
            case TokenType::OpAndAssign:
            case TokenType::OpOrAssign:
            case TokenType::OpXorAssign:
            case TokenType::OpShiftLeftAssign:
            case TokenType::OpShiftRightAssign:
            case TokenType::OpExpAssign:
                UnsupportedCallArgs(t, nullptr);
                return expr;
            case TokenType::SymbolComma:
                IgnoreCallArgs(expr);
                return TryTuple(expr);
            case TokenType::Identifier:
                MustNotHave(expr, t, nullptr);
                TryParsing(expr, nullptr, TryName, noAssign, callArgs);
                continue;
            case TokenType::NumberFloat:
            case TokenType::NumberInteger:
            case TokenType::NumberHex:
            case TokenType::String:
                MustNotHave(expr, t, nullptr);
                TryParsing(expr, nullptr, TryConstant);
                continue;
            case TokenType::SymbolParentesesOpen:
            {
                MustNotHave(expr, t, nullptr);
                ReadNextToken();
                TryParsing(expr, nullptr, TryExpression, true, false, false);
                TryToken(t, TokenType::SymbolParentesesClose);
                return expr;
            }
            case TokenType::SymbolBracketOpen:
            {
                MustNotHave(expr, t, nullptr);
                ReadNextToken();
                TryParsing(expr, nullptr, TryList);
                TryToken(t, TokenType::SymbolBracketClose);
                continue;
            }
            case TokenType::SymbolBraceOpen:
            {
                MustNotHave(expr, t, nullptr);
                ReadNextToken();
                TryParsing(expr, nullptr, TryDict);
                TryToken(t, TokenType::SymbolBraceClose);
                continue;
            }
            case TokenType::OpInv:
                TryParsing(expr, nullptr, TryUnaryOp);
                continue;
            case TokenType::OpAdd:
            case TokenType::OpSub:
                if (!expr)
                {
                    TryParsing(expr, nullptr, TryUnaryOp);
                    continue;
                }
            case TokenType::OpMul:
            case TokenType::OpDiv:
            case TokenType::OpExp:
            case TokenType::OpMod:
            case TokenType::OpXor:
            case TokenType::OpFloorDiv:
            case TokenType::OpShiftLeft:
            case TokenType::OpShiftRight:
            case TokenType::OpAnd:
            case TokenType::OpOr:
                MustHave(expr, t, nullptr);
                return TryBinOp(expr);
            case TokenType::KeywordAnd:
            case TokenType::KeywordOr:
                MustHave(expr, t, nullptr);
                return TryBoolOp(expr);
            case TokenType::KeywordNot:
                if (!expr)
                {
                    return TryUnaryOp();
                }
            case TokenType::OpEqual:
            case TokenType::OpNotEqual:
            case TokenType::OpGreatEqual:
            case TokenType::OpLessEqual:
            case TokenType::OpGreat:
            case TokenType::OpLess:
            case TokenType::KeywordIs:
            case TokenType::KeywordIn:
                MustHave(expr, t, nullptr);
                return TryCompare(expr);
            case TokenType::SymbolColon:
                if (allowColon)
                {
                    return expr;
                }
            default:
                UnexpectedToken(t, nullptr);
            }
        }
        return nullptr;
    }

    expr_t Parser::TryBoolOp(expr_t left)
    {
        Token t = ReadNextToken();
        TryParsingAuto(right, nullptr, TryExpression, true, false, false);
        switch (t.Type)
        {
        case TokenType::KeywordAnd:
            return BoolOp(left, boolop_t::kAnd, right, left->lineno, left->col_offset);
        case TokenType::KeywordOr:
            return BoolOp(left, boolop_t::kOr, right, left->lineno, left->col_offset);
        default:
            UnexpectedToken(t, nullptr);
        }
        return nullptr;
    }

    expr_t Parser::TryBinOp(expr_t left)
    {
        Token t = ReadNextToken();
        TryParsingAuto(right, nullptr, TryExpression, true, false, false);
        switch (t.Type)
        {
        case TokenType::OpAdd:
            return BinOp(left, operator_t::kAdd, right, left->lineno, left->col_offset);
        case TokenType::OpSub:
            return BinOp(left, operator_t::kSub, right, left->lineno, left->col_offset);
        case TokenType::OpMul:
            return BinOp(left, operator_t::kMult, right, left->lineno, left->col_offset);
        case TokenType::OpDiv:
            return BinOp(left, operator_t::kDiv, right, left->lineno, left->col_offset);
        case TokenType::OpExp:
            return BinOp(left, operator_t::kPow, right, left->lineno, left->col_offset);
        case TokenType::OpMod:
            return BinOp(left, operator_t::kMod, right, left->lineno, left->col_offset);
        case TokenType::OpXor:
            return BinOp(left, operator_t::kBitXor, right, left->lineno, left->col_offset);
        case TokenType::OpFloorDiv:
            return BinOp(left, operator_t::kFloorDiv, right, left->lineno, left->col_offset);
        case TokenType::OpShiftLeft:
            return BinOp(left, operator_t::kLShift, right, left->lineno, left->col_offset);
        case TokenType::OpShiftRight:
            return BinOp(left, operator_t::kRShift, right, left->lineno, left->col_offset);
        case TokenType::OpAnd:
            return BinOp(left, operator_t::kBitAnd, right, left->lineno, left->col_offset);
        case TokenType::OpOr:
            return BinOp(left, operator_t::kBitOr, right, left->lineno, left->col_offset);
        default:
            UnexpectedToken(t, nullptr);
        }
        return nullptr;
    }

    expr_t Parser::TryUnaryOp()
    {
        Token t = ReadNextToken();
        PinPosition(t, lineno, colno);
        unaryop_t op;
        expr_t operand = nullptr;

        switch (t.Type)
        {
        case TokenType::OpInv:
            op = unaryop_t::kInvert;
            break;
        case TokenType::OpAdd:
            op = unaryop_t::kUAdd;
            break;
        case TokenType::OpSub:
            op = unaryop_t::kUSub;
            break;
        case TokenType::KeywordNot:
            op = unaryop_t::kNot;
            break;
        default:
            UnexpectedToken(t, nullptr);
        }

        t = PeekNextToken();
        switch (t.Type)
        {
        case TokenType::SymbolParentesesOpen:
            ReadNextToken();
            TryParsing(operand, nullptr, TryExpression, true, false, false);
            TryToken(t, TokenType::SymbolParentesesClose);
            break;
        case TokenType::NumberFloat:
        case TokenType::NumberInteger:
        case TokenType::NumberHex:
            TryParsing(operand, nullptr, TryConstant);
            break;
        case TokenType::KeywordIn:
            ReadNextToken();
            TryParsing(operand, nullptr, TryExpression, true, false, false);
            break;
        case TokenType::KeywordFalse:
            operand = Name("False", expr_context_t::kLoad, t.Line, t.Column);
            break;
        case TokenType::KeywordTrue:
            operand = Name("True", expr_context_t::kLoad, t.Line, t.Column);
            break;
        default:
            UnexpectedToken(t, nullptr);
        }

        return UnaryOp(op, operand, lineno, colno);
    }

    expr_t Parser::TryLambda()
    {
        return nullptr;
    }

    expr_t Parser::TryIfExp()
    {
        return nullptr;
    }

    expr_t Parser::TryDict()
    {
        return nullptr;
    }

    expr_t Parser::TryCompare(expr_t left)
    {
        Token t = ReadNextToken();
        TryParsingAuto(right, nullptr, TryExpression, true, false, false);
        switch (t.Type)
        {
        case TokenType::OpEqual:
            return Compare(left, cmpop_t::kEq, right, left->lineno, left->col_offset);
        case TokenType::OpNotEqual:
            return Compare(left, cmpop_t::kNotEq, right, left->lineno, left->col_offset);
        case TokenType::OpGreatEqual:
            return Compare(left, cmpop_t::kGtE, right, left->lineno, left->col_offset);
        case TokenType::OpLessEqual:
            return Compare(left, cmpop_t::kLtE, right, left->lineno, left->col_offset);
        case TokenType::OpGreat:
            return Compare(left, cmpop_t::kGt, right, left->lineno, left->col_offset);
        case TokenType::OpLess:
            return Compare(left, cmpop_t::kLt, right, left->lineno, left->col_offset);
        case TokenType::KeywordIs:
            return Compare(left, cmpop_t::kIs, right, left->lineno, left->col_offset);
        case TokenType::KeywordIn:
            return Compare(left, cmpop_t::kIn, right, left->lineno, left->col_offset);
        default:
            UnexpectedToken(t, nullptr);
        }
        return nullptr;
    }

    expr_t Parser::TryCall(expr_t func)
    {
        Token t;
        TryToken(t, TokenType::SymbolParentesesOpen);
        std::vector<expr_t> args;
        for (;;)
        {
            auto t = PeekNextToken();

            switch (t.Type)
            {
            case TokenType::End:
            case TokenType::NewLine:
                ExpectedToken(t, ')', nullptr);
            case TokenType::SymbolParentesesClose:
                ReadNextToken();
                return Call(func, args, func->lineno, func->col_offset);
            case TokenType::SymbolComma:
                ReadNextToken();
                continue;
            default:
                TryParsingAuto(arg, nullptr, TryExpression, true, true, false);
                args.push_back(arg);
                break;
            }
        }

        return nullptr;
    }

    expr_t Parser::TryFormattedValue()
    {
        return nullptr;
    }

    expr_t Parser::TryConstant()
    {
        auto t = ReadNextToken();
        switch (t.Type)
        {
        case TokenType::NumberFloat:
            return Constant(constant(std::stod(t.Value)), "float", t.Line, t.Column);
        case TokenType::NumberInteger:
            return Constant(constant(std::stoi(t.Value)), "int", t.Line, t.Column);
        case TokenType::NumberHex:
            return Constant(constant(t.Value), "hex", t.Line, t.Column);
        case TokenType::String:
            return Constant(constant(t.Value), "string", t.Line, t.Column);
        default:
            ANTOMIC_ERROR("Unexpected token on line {0}, column {1}: {2}", t.Line, t.Column, t.Value);
            return nullptr;
        }
        return nullptr;
    }

    expr_t Parser::TryAttribute(expr_t value)
    {
        Token t;
        TryToken(t, TokenType::SymbolPeriod);
        TryIdentifier(t, attr);
        PinPosition(t, lineno, colno);
        t = PeekNextToken();
        switch (t.Type)
        {
        case TokenType::OpAssign:
        case TokenType::OpAddAssign:
        case TokenType::OpSubAssign:
        case TokenType::OpMulAssign:
        case TokenType::OpDivAssign:
        case TokenType::OpFloorDivAssign:
        case TokenType::OpModAssign:
        case TokenType::OpAndAssign:
        case TokenType::OpOrAssign:
        case TokenType::OpXorAssign:
        case TokenType::OpShiftLeftAssign:
        case TokenType::OpShiftRightAssign:
        case TokenType::OpExpAssign:
            return Attribute(value, attr, expr_context_t::kStore, lineno, colno);
        case TokenType::SymbolParentesesOpen:
            return TryCall(Attribute(value, attr, expr_context_t::kLoad, lineno, colno));
        default:
            return Attribute(value, attr, expr_context_t::kLoad, lineno, colno);
        }

        return nullptr;
    }

    expr_t Parser::TrySubscript(expr_t value)
    {
        Token t;
        TryToken(t, TokenType::SymbolBracketOpen);
        PinPosition(t, lineno, colno);
        TryParsingAuto(slice, nullptr, TryIndex);
        TryToken(t, TokenType::SymbolBracketClose);
        t = PeekNextToken();
        switch (t.Type)
        {
        case TokenType::SymbolBracketOpen:
            return TrySubscript(Subscript(value, slice, expr_context_t::kStore, lineno, colno));
        default:
            return Subscript(value, slice, expr_context_t::kLoad, lineno, colno);
        }
    }

    expr_t Parser::TryName(bool noAssign, bool ignoreComma)
    {
        Token t;
        TryIdentifier(t, identifier);
        PinPosition(t, lineno, colno);
        t = PeekNextToken();
        switch (t.Type)
        {
        case TokenType::SymbolPeriod:
            return TryAttribute(Name(identifier, expr_context_t::kLoad, lineno, colno));
        case TokenType::SymbolBracketOpen:
            return TrySubscript(Name(identifier, expr_context_t::kLoad, lineno, colno));
        case TokenType::SymbolComma:
            if (ignoreComma)
            {
                return Name(identifier, expr_context_t::kLoad, lineno, colno);
            }
            return TryTupleNames(identifier, lineno, colno);
        case TokenType::OpAssign:
        case TokenType::OpAddAssign:
        case TokenType::OpSubAssign:
        case TokenType::OpMulAssign:
        case TokenType::OpDivAssign:
        case TokenType::OpFloorDivAssign:
        case TokenType::OpModAssign:
        case TokenType::OpAndAssign:
        case TokenType::OpOrAssign:
        case TokenType::OpXorAssign:
        case TokenType::OpShiftLeftAssign:
        case TokenType::OpShiftRightAssign:
        case TokenType::OpExpAssign:
            UnsupportedCallArgs(t, nullptr);
            return Name(identifier, expr_context_t::kStore, lineno, colno);
        case TokenType::SymbolParentesesOpen:
            return TryCall(Name(identifier, expr_context_t::kLoad, lineno, colno));
        default:
            return Name(identifier, expr_context_t::kLoad, lineno, colno);
        }
    }

    expr_t Parser::TryList()
    {
        std::vector<expr_t> names;
        for (;;)
        {
            auto t = PeekNextToken();
            switch (t.Type)
            {
            case TokenType::NewLine:
            case TokenType::Identation:
            case TokenType::SymbolComma:
                ReadNextToken();
                continue;
            case TokenType::KeywordNone:
                ReadNextToken();
                names.push_back(Name("None", expr_context_t::kLoad, t.Line, t.Column));
                continue;
            case TokenType::KeywordTrue:
                ReadNextToken();
                names.push_back(Name("True", expr_context_t::kLoad, t.Line, t.Column));
                continue;
            case TokenType::KeywordFalse:
                ReadNextToken();
                names.push_back(Name("False", expr_context_t::kLoad, t.Line, t.Column));
                continue;
            case TokenType::NumberFloat:
            case TokenType::NumberInteger:
            case TokenType::NumberHex:
            case TokenType::String:
            {
                TryParsingAuto(expr, nullptr, TryConstant);
                names.push_back(expr);
                continue;
            }
            case TokenType::Identifier:
            {
                TryIdentifier(t, identifier);
                names.push_back(Name(identifier, expr_context_t::kLoad, t.Line, t.Column));
                continue;
            }
            default:
                return List(names, expr_context_t::kLoad, t.Line, t.Column);
            }
        }

        return nullptr;
    }

    expr_t Parser::TryTupleNames(identifier first, uint32_t line, uint32_t column)
    {
        std::vector<IdentifierTupleInfo> iList;
        iList.push_back({first, line, column});

        for (;;)
        {
            auto t = PeekNextToken();
            switch (t.Type)
            {
            case TokenType::Identation:
            case TokenType::SymbolComma:
                ReadNextToken();
                continue;
            case TokenType::Identifier:
            {
                TryIdentifier(t, identifier);
                iList.push_back({identifier, t.Line, t.Column});
                continue;
            }
            case TokenType::OpAssign:
            case TokenType::OpAddAssign:
            case TokenType::OpSubAssign:
            case TokenType::OpMulAssign:
            case TokenType::OpDivAssign:
            case TokenType::OpFloorDivAssign:
            case TokenType::OpModAssign:
            case TokenType::OpAndAssign:
            case TokenType::OpOrAssign:
            case TokenType::OpXorAssign:
            case TokenType::OpShiftLeftAssign:
            case TokenType::OpShiftRightAssign:
            case TokenType::OpExpAssign:
            {
                std::vector<expr_t> exprList;
                for (IdentifierTupleInfo i : iList)
                {
                    exprList.push_back(Name(i.Identifier, expr_context_t::kStore, i.Line, i.Column));
                }

                return Tuple(exprList, expr_context_t::kStore, line, column);
            }
            default:
            {
                std::vector<expr_t> exprList;
                for (IdentifierTupleInfo i : iList)
                {
                    exprList.push_back(Name(i.Identifier, expr_context_t::kLoad, i.Line, i.Column));
                }

                return Tuple(exprList, expr_context_t::kLoad, line, column);
            }
            }
        }

        return nullptr;
    }

    expr_t Parser::TryTuple(expr_t first)
    {
        std::vector<expr_t> names;
        names.push_back(first);
        for (;;)
        {
            auto t = PeekNextToken();
            switch (t.Type)
            {
            case TokenType::Identation:
            case TokenType::SymbolComma:
                ReadNextToken();
                continue;
            case TokenType::KeywordNone:
                ReadNextToken();
                names.push_back(Name("None", expr_context_t::kLoad, t.Line, t.Column));
                continue;
            case TokenType::KeywordTrue:
                ReadNextToken();
                names.push_back(Name("True", expr_context_t::kLoad, t.Line, t.Column));
                continue;
            case TokenType::KeywordFalse:
                ReadNextToken();
                names.push_back(Name("False", expr_context_t::kLoad, t.Line, t.Column));
                continue;
            case TokenType::NumberFloat:
            case TokenType::NumberInteger:
            case TokenType::NumberHex:
            case TokenType::String:
            {
                TryParsingAuto(expr, nullptr, TryConstant);
                names.push_back(expr);
            }
                continue;
            case TokenType::Identifier:
            {
                TryIdentifier(t, identifier);
                names.push_back(Name(identifier, expr_context_t::kLoad, t.Line, t.Column));
                continue;
            }
            default:
                return Tuple(names, expr_context_t::kLoad, t.Line, t.Column);
            }
        }

        return nullptr;
    }

    expr_t Parser::TryIndex()
    {
        expr_t index = nullptr;
        for (;;)
        {
            auto t = PeekNextToken();
            switch (t.Type)
            {
            case TokenType::SymbolBracketClose:
                MustHave(index, t, nullptr);
                return Index(index, index->lineno, index->col_offset);
            case TokenType::Identifier:
            {
                MustNotHave(index, t, nullptr);
                TryIdentifier(t, identifier);
                index = Name(identifier, expr_context_t::kLoad, t.Line, t.Column);
                continue;
            }
            case TokenType::NumberInteger:
            case TokenType::NumberFloat:
            case TokenType::NumberHex:
            case TokenType::String:
                MustNotHave(index, t, nullptr);
                TryParsing(index, nullptr, TryConstant);
                continue;
            case TokenType::SymbolColon:
                return TrySlice(index);
            case TokenType::SymbolComma:
                MustHave(index, t, nullptr);
                ReadNextToken();
                TryParsing(index, nullptr, TryTuple, index);
                return Index(index, t.Line, t.Column);
            default:
                UnexpectedToken(t, nullptr);
            }
        }

        return nullptr;
    }

    expr_t Parser::TrySlice(expr_t first)
    {
        expr_t lower = nullptr;
        expr_t upper = nullptr;
        expr_t step = nullptr;
        for (;;)
        {
        }

        return nullptr;
    }

    excepthandler_t Parser::TryExceptHandler()
    {
        return nullptr;
    }

    alias_t Parser::TryAlias()
    {
        return nullptr;
    }
}