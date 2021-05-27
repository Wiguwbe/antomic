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

#define AssertToken(x, y) \
    x = ReadNextToken();  \
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

#define TryParsing(x, y) \
    x = y();             \
    if (!x)              \
    {                    \
        return nullptr;  \
    }

#define TryParsingAuto(x, y) \
    auto x = y();            \
    if (!x)                  \
    {                        \
        return nullptr;      \
    }

#define PinPosition(x, y, z) \
    auto y = x.Line;         \
    auto z = x.Column;

#define ParsingError(x)                                                                       \
    ANTOMIC_ERROR("Parsing error on line {0}, column {1}: '{2}'", x.Line, x.Column, x.Value); \
    return nullptr;

#define WrongIdentation(x)                                                                                  \
    ANTOMIC_ERROR("Wrong identation on line {0}, column {1}: got '{2}'", x.Line, x.Column, x.Value.size()); \
    return nullptr;

#define MissingBody(x, y)                                                                 \
    ANTOMIC_ERROR("Missing body for '{0}' on line {1}, column {2}", y, x.Line, x.Column); \
    return nullptr;

#define ExpectedToken(x, y)                                                                                           \
    ANTOMIC_ERROR("Unexpected token on line {0}, column {1}: got '{2}', expected {3}", t.Line, t.Column, t.Value, y); \
    return nullptr;

#define UnexpectedToken(x)                                                                       \
    ANTOMIC_ERROR("Unexpected token on line {0}, column {1}: '{2}'", t.Line, t.Column, t.Value); \
    return nullptr;

#define MustNotHave(x, y) \
    if (x)                \
    {                     \
        ParsingError(y);  \
    }

#define MustHave(x, y)   \
    if (!x)              \
    {                    \
        ParsingError(y); \
    }

#define TryBody(x, y, z, a, b, ...)                                    \
    std::vector<stmt_t> body;                                          \
    for (;;)                                                           \
    {                                                                  \
        auto x = PeekNextToken();                                      \
                                                                       \
        switch (x.Type)                                                \
        {                                                              \
        case TokenType::End:                                           \
            if (body.empty())                                          \
            {                                                          \
                MissingBody(x, z);                                     \
            }                                                          \
            return y(__VA_ARGS__, body, a, b);                         \
        case TokenType::Identation:                                    \
            if (x.Value.size() < CurrentIdentation())                  \
            {                                                          \
                if (body.empty())                                      \
                {                                                      \
                    MissingBody(x, z);                                 \
                }                                                      \
                PopIdentation();                                       \
                return y(__VA_ARGS__, body, a, b);                     \
            }                                                          \
            if (x.Value.size() > CurrentIdentation() && !body.empty()) \
            {                                                          \
                WrongIdentation(x);                                    \
            }                                                          \
            ReadNextToken();                                           \
            PushIdentation((uint8_t)x.Value.size());                   \
            continue;                                                  \
        case TokenType::Invalid:                                       \
            ParsingError(x);                                           \
        case TokenType::Comment:                                       \
            ReadNextToken();                                           \
            continue;                                                  \
        default:                                                       \
            if (x.Column < CurrentIdentation())                        \
            {                                                          \
                if (body.empty())                                      \
                {                                                      \
                    MissingBody(x, z);                                 \
                }                                                      \
                PopIdentation();                                       \
                return y(__VA_ARGS__, body, a, b);                     \
            }                                                          \
            TryParsingAuto(stmt, TryStatement);                        \
            body.push_back(stmt);                                      \
            break;                                                     \
        }                                                              \
    }                                                                  \
    return nullptr;

namespace Antomic
{
    mod_t Parser::FromFile(const std::string &name)
    {
        mLexer = CreateRef<Lexer>(name);
        std::vector<stmt_t> body;
        for (;;)
        {
            auto t = PeekNextToken();
            switch (t.Type)
            {
            case TokenType::End:
                ReadNextToken();
                return Module(body);
            case TokenType::Invalid:
                ReadNextToken();
                ParsingError(t);
                return nullptr;
            case TokenType::Comment:
                ReadNextToken();
                continue;
            default:
                TryParsingAuto(stmt, TryStatement);
                body.push_back(stmt);
                break;
            }
        }

        return nullptr;
    }

    mod_t Parser::FromExpression(const std::string &expression)
    {
        auto reader = Reader::FromString(expression, "expression");
        mLexer = CreateRef<Lexer>(reader);
        TryParsingAuto(expr, TryExpression);
        return Expression(expr);
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
                return TryIf();
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
        Token t;
        AssertToken(t, "def");
        PinPosition(t, lineno, colno);
        TryIdentifier(t, identifier);
        TryToken(t, TokenType::SymbolParentesesOpen);
        TryParsingAuto(args, TryArguments);
        TryToken(t, TokenType::SymbolColon);
        TryToken(t, TokenType::NewLine);
        TryBody(t, FunctionDef, identifier, lineno, colno, identifier, args);
    }

    stmt_t Parser::TryClassDef()
    {
        Token t;
        AssertToken(t, "class");
        PinPosition(t, lineno, colno);
        TryIdentifier(t, identifier);
        TryToken(t, TokenType::SymbolParentesesOpen);
        TryParsingAuto(baseclasses, TryBaseClasses);
        TryToken(t, TokenType::SymbolColon);
        TryToken(t, TokenType::NewLine);
        TryBody(t, ClassDef, identifier, lineno, colno, identifier, baseclasses);
    }

    stmt_t Parser::TryReturn()
    {
        Token t;
        AssertToken(t, "return");
        TryParsingAuto(expr, TryExpression);
        TryToken(t, TokenType::NewLine);
        return Return(expr, t.Line, t.Column);
    }

    stmt_t Parser::TryDelete()
    {
        Token t;
        AssertToken(t, "del");
        TryParsingAuto(expr, TryDeleteExpr);
        TryToken(t, TokenType::NewLine);
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
        TryParsingAuto(expr, TryExpression);
        TryToken(t, TokenType::NewLine);
        return Assign(target, expr, t.Line, t.Column);
    }

    stmt_t Parser::TryAugAssign(expr_t target)
    {
        Token t = ReadNextToken();
        TryParsingAuto(expr, TryExpression);
        TryToken(t, TokenType::NewLine);

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
            UnexpectedToken(t);
        }

        return nullptr;
    }

    stmt_t Parser::TryFor()
    {
        Token t;
        AssertToken(t, "for");
        PinPosition(t, lineno, colno);
        TryParsingAuto(target, TryForTarget);
        TryToken(t, TokenType::KeywordIn);
        TryParsingAuto(iter, TryForIter);
        TryToken(t, TokenType::SymbolComma);
        TryToken(t, TokenType::NewLine);
        TryBody(t, For, "for", lineno, colno, target, iter);
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
                ExpectedToken(t, "in");
            case TokenType::KeywordIn:
                if (names.size() == 0)
                {
                    ParsingError(t);
                }
                return names.size() == 1 ? names.front() : Tuple(names, expr_context_t::kStore, t.Line, t.Column);
            case TokenType::SymbolComma:
                continue;
            case TokenType::Identifier:
                names.push_back(Name(t.Value, expr_context_t::kStore, t.Line, t.Column));
                continue;
            default:
                ParsingError(t);
            }
        }

        return nullptr;
    }

    expr_t Parser::TryForIter()
    {
        return nullptr;
    }

    stmt_t Parser::TryWhile()
    {
        Token t;
        AssertToken(t, "while");
        PinPosition(t, lineno, colno);
        TryParsingAuto(test, TryExpression);
        TryToken(t, TokenType::SymbolComma);
        TryToken(t, TokenType::NewLine);
        TryBody(t, While, "while", lineno, colno, test);
    }

    expr_t ParserTryWhileTest()
    {
        return nullptr;
    }

    stmt_t Parser::TryIf()
    {
        Token t;
        AssertToken(t, "if");
        return nullptr;
    }

    stmt_t Parser::TryRaise()
    {
        Token t;
        AssertToken(t, "raise");
        return nullptr;
    }

    stmt_t Parser::TryTry()
    {
        Token t;
        AssertToken(t, "try");
        return nullptr;
    }

    stmt_t Parser::TryAssert()
    {
        Token t;
        AssertToken(t, "assert");
        return nullptr;
    }

    stmt_t Parser::TryImport()
    {
        Token t;
        AssertToken(t, "import");

        return nullptr;
    }

    stmt_t Parser::TryImportFrom()
    {
        Token t;
        AssertToken(t, "from");
        return nullptr;
    }

    // a = b + (c+1)
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
                MustHave(expr, t);
                return Expr(expr, expr->lineno, expr->col_offset);
            case TokenType::OpAssign:
                MustHave(expr, t);
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
                MustHave(expr, t);
                return TryAugAssign(expr);
            default:
                TryParsing(expr, TryExpression);
            }
        }
        return nullptr;
    }

    stmt_t Parser::TryPass()
    {
        Token t;
        AssertToken(t, "pass");
        return Pass(t.Line, t.Column);
    }

    stmt_t Parser::TryBreak()
    {
        Token t;
        AssertToken(t, "pass");
        return Break(t.Line, t.Column);
    }

    stmt_t Parser::TryContinue()
    {
        Token t;
        AssertToken(t, "pass");
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
                ExpectedToken(t, ')');
            case TokenType::SymbolParentesesClose:
                ReadNextToken();
                return arguments(args);
            case TokenType::SymbolComma:
                ReadNextToken();
                continue;
            default:
                TryParsingAuto(arg, TryArg);
                args.push_back(arg);
                break;
            }
        }

        return arguments(args);
    }

    arg_t Parser::TryArg()
    {
        Token t;
        PinPosition(t, lineno, colno);
        TryIdentifier(t, identifier);

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
                ExpectedToken(t, ')');
            case TokenType::SymbolParentesesClose:
                ReadNextToken();
                return baseclasses(bases);
            case TokenType::SymbolComma:
                continue;
            default:
                TryParsingAuto(base, TryBaseClass);
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

    // a = b + (c+1)
    expr_t Parser::TryExpression()
    {
        expr_t expr = nullptr;
        for (;;)
        {
            auto t = PeekNextToken();
            switch (t.Type)
            {
            case TokenType::End:
            case TokenType::SymbolParentesesClose:
            case TokenType::NewLine:
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
                return expr;
            case TokenType::Identifier:
                MustNotHave(expr, t);
                TryParsing(expr, TryName);
                continue;
            case TokenType::NumberFloat:
            case TokenType::NumberInteger:
            case TokenType::NumberHex:
            case TokenType::String:
                MustNotHave(expr, t);
                TryParsing(expr, TryConstant);
                continue;
            case TokenType::SymbolParentesesOpen:
            {
                MustNotHave(expr, t);
                ReadNextToken();
                TryParsing(expr, TryExpression);
                TryToken(t, TokenType::SymbolParentesesClose);
                return expr;
            }
            case TokenType::OpInv:
                return TryUnaryOp();
            case TokenType::OpAdd:
            case TokenType::OpSub:
                if (!expr)
                {
                    return TryUnaryOp();
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
                MustHave(expr, t);
                return TryBinOp(expr);
            case TokenType::KeywordAnd:
            case TokenType::KeywordOr:
                MustHave(expr, t);
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
                MustHave(expr, t);
                return TryCompare(expr);
            default:
                UnexpectedToken(t);
            }
        }
        return nullptr;
    }

    expr_t Parser::TryBoolOp(expr_t left)
    {
        Token t = ReadNextToken();
        TryParsingAuto(right, TryExpression);
        switch (t.Type)
        {
        case TokenType::KeywordAnd:
            return BoolOp(left, boolop_t::kAnd, right, left->lineno, left->col_offset);
        case TokenType::KeywordOr:
            return BoolOp(left, boolop_t::kOr, right, left->lineno, left->col_offset);
        default:
            UnexpectedToken(t);
        }
        return nullptr;
    }

    expr_t Parser::TryBinOp(expr_t left)
    {
        Token t = ReadNextToken();
        TryParsingAuto(right, TryExpression);
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
            UnexpectedToken(t);
        }
        return nullptr;
    }

    expr_t Parser::TryUnaryOp()
    {
        return nullptr;
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

    expr_t Parser::TrySet()
    {
        return nullptr;
    }

    expr_t Parser::TryCompare(expr_t left)
    {
        return nullptr;
    }

    expr_t Parser::TryCall(expr_t func)
    {
        std::vector<expr_t> args;
        for (;;)
        {
            auto t = PeekNextToken();

            switch (t.Type)
            {
            case TokenType::End:
            case TokenType::NewLine:
                ExpectedToken(t, ')');
            case TokenType::SymbolParentesesClose:
                ReadNextToken();
                return Call(func, args, func->lineno, func->col_offset);
            case TokenType::SymbolComma:
                continue;
            default:
                TryParsingAuto(arg, TryCallArg);
                args.push_back(arg);
                break;
            }
        }

        return nullptr;
    }

    expr_t Parser::TryCallArg()
    {
        return nullptr;
    }

    expr_t Parser::TryFormattedValue()
    {
        return nullptr;
    }

    expr_t Parser::TryJoinedStr()
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
        PinPosition(t, lineno, colno);
        TryIdentifier(t, attr);

        t = PeekNextToken();
        if (t.Type == TokenType::SymbolParentesesOpen)
        {
            return Attribute(value, attr, expr_context_t::kLoad, lineno, colno);
        }
        return Attribute(value, attr, expr_context_t::kStore, lineno, colno);
    }

    expr_t Parser::TrySubscript()
    {
        return nullptr;
    }

    expr_t Parser::TryStarred()
    {
        return nullptr;
    }

    expr_t Parser::TryName()
    {
        Token t;
        TryIdentifier(t, identifier);
        PinPosition(t, lineno, colno);
        t = PeekNextToken();
        switch (t.Type)
        {
        case TokenType::SymbolPeriod:
            return TryAttribute(Name(identifier, expr_context_t::kLoad, lineno, colno));
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
            return Name(identifier, expr_context_t::kStore, lineno, colno);
        case TokenType::SymbolParentesesOpen:
        {
            ReadNextToken();
            auto value = TryCall(Name(identifier, expr_context_t::kLoad, lineno, colno));
            auto t = PeekNextToken();
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
                UnexpectedToken(t);
            }
            return value;
        }
        default:
            return Name(identifier, expr_context_t::kLoad, lineno, colno);
        }
    }

    expr_t Parser::TryList()
    {
        return nullptr;
    }

    expr_t Parser::TryTuple()
    {
        return nullptr;
    }

    expr_t Parser::TrySlice()
    {
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