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

static std::map<std::string, Antomic::kind_t> sTypesMap{
	{"string", Antomic::kind_t::kString},
	{"int", Antomic::kind_t::kInt},
	{"float", Antomic::kind_t::kFloat},
	{"object", Antomic::kind_t::kObject},
};

#define AssertTokenValue(x, y)                                                                     \
	x = ReadNextToken();                                                                           \
	ANTOMIC_ASSERT(x.Value == y, "Wrong token");

#define AssertTokenType(x, y)                                                                      \
	x = ReadNextToken();                                                                           \
	ANTOMIC_ASSERT(x.Type == y, "Wrong token");

#define EndOrNewLine(x)                                                                            \
	x = PeekNextToken();                                                                           \
	if(x.Type != TokenType::End && x.Type != TokenType::NewLine)                                   \
	{                                                                                              \
		ANTOMIC_ERROR("Unexpected token on line {0}, column {1}: {2}", x.Line, x.Column, x.Value); \
		return nullptr;                                                                            \
	}                                                                                              \
	x = ReadNextToken();

#define PeekToken(x, y)                                                                            \
	x = PeekNextToken();                                                                           \
	if(x.Type != y)                                                                                \
	{                                                                                              \
		ANTOMIC_ERROR("Unexpected token on line {0}, column {1}: {2}", x.Line, x.Column, x.Value); \
		return nullptr;                                                                            \
	}

#define TryToken(x, y)                                                                             \
	PeekToken(x, y);                                                                               \
	x = ReadNextToken();

#define TryIdentifier(x, y)                                                                        \
	TryToken(x, TokenType::Identifier);                                                            \
	auto y = x.Value;

#define TryParsing(x, y, z, ...)                                                                   \
	x = z(__VA_ARGS__);                                                                            \
	if(!x)                                                                                         \
	{                                                                                              \
		return y;                                                                                  \
	}

#define TryParsingAuto(x, y, z, ...)                                                               \
	auto x = z(__VA_ARGS__);                                                                       \
	if(!x)                                                                                         \
	{                                                                                              \
		return y;                                                                                  \
	}

#define PinPosition(x, y, z)                                                                       \
	auto y = x.Line;                                                                               \
	auto z = x.Column;

#define ParsingError(x, y)                                                                         \
	ANTOMIC_ERROR("Parsing error on line {0}, column {1}: '{2}'", x.Line, x.Column, x.Value);      \
	return y;

#define WrongIdentation(x, y)                                                                      \
	ANTOMIC_ERROR(                                                                                 \
		"Wrong identation on line {0}, column {1}: got '{2}'", x.Line, x.Column, x.Value.size());  \
	return y;

#define MissingBody(x, y, z)                                                                       \
	ANTOMIC_ERROR("Missing body for '{0}' on line {1}, column {2}", y, x.Line, x.Column);          \
	return z;

#define ExpectedToken(x, y, z)                                                                     \
	ANTOMIC_ERROR("Unexpected token on line {0}, column {1}: got '{2}', expected {3}",             \
				  x.Line,                                                                          \
				  x.Column,                                                                        \
				  x.Value,                                                                         \
				  y);                                                                              \
	return z;

#define UnexpectedToken(x, y)                                                                      \
	ANTOMIC_ERROR("Unexpected token on line {0}, column {1}: '{2}'", t.Line, t.Column, t.Value);   \
	return y;

#define MustNotHave(x, y, z)                                                                       \
	if(x)                                                                                          \
	{                                                                                              \
		ParsingError(y, z);                                                                        \
	}

#define MustHave(x, y, z)                                                                          \
	if(!x)                                                                                         \
	{                                                                                              \
		ParsingError(y, z);                                                                        \
	}

#define UnsupportedCallArgs(x, y)                                                                  \
	if(noAssign)                                                                                   \
	{                                                                                              \
		UnexpectedToken(x, y);                                                                     \
	}

#define IgnoreCallArgs(x)                                                                          \
	if(callArgs)                                                                                   \
	{                                                                                              \
		return x;                                                                                  \
	}

#define HaveBodyNoEmpty(x, y)                                                                      \
	if(!TryBody(x, y))                                                                             \
	{                                                                                              \
		return nullptr;                                                                            \
	}

#define EmptyStmtBody                                                                              \
	std::vector<stmt_t> { }

#define EmptyHandlers                                                                              \
	std::vector<excepthandler_t> { }

#define EmptyExprList                                                                              \
	std::vector<expr_t> { }

#define New(x, y, z, ...) auto x = dynamic_cast<y>(z(__VA_ARGS__));

#define ClearStack(x)                                                                              \
	while(!x.empty())                                                                              \
	{                                                                                              \
		x.pop();                                                                                   \
	}

namespace Antomic
{
	mod_t Parser::FromFile(const std::string& name)
	{
		mLexer = CreateRef<Lexer>(name);
		std::vector<stmt_t> body;
		New(mod, Module_t, Module, EmptyStmtBody);
		HaveBodyNoEmpty(name, mod->body);
		ClearStack(mState.IfStack);
		ClearStack(mState.TryStack);
		return mod;
	}

	mod_t Parser::FromExpression(const std::string& expression)
	{
		auto reader = Reader::FromString(expression, "expression");
		mLexer = CreateRef<Lexer>(reader);
		TryParsingAuto(expr, nullptr, TryExpression);
		ClearStack(mState.IfStack);
		ClearStack(mState.TryStack);
		return Expression(expr);
	}

	bool Parser::TryBody(const std::string& name, std::vector<stmt_t>& body, bool allowEmpty)
	{
		for(;;)
		{
			auto t = PeekNextToken();
			switch(t.Type)
			{
			case TokenType::End:
				if(body.empty() && !allowEmpty)
				{
					MissingBody(t, name, false);
				}
				return true;
			case TokenType::Identation:
				if(t.Value.size() < CurrentIdentation())
				{
					if(body.empty() && !allowEmpty)
					{
						MissingBody(t, name, false);
					}

					PopIdentation();
					return true;
				}
				if(t.Value.size() > CurrentIdentation() && !body.empty())
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
			case TokenType::KeywordElIf: {
				MustNotHave(mState.IfStack.empty(), t, false);
				TryParsingAuto(stmt, false, TryStatement);
				continue;
			}
			case TokenType::KeywordExcept:
			case TokenType::KeywordFinally: {
				MustNotHave(mState.TryStack.empty(), t, false);
				TryParsingAuto(stmt, false, TryStatement);
				continue;
			}
			default: {
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
		for(;;)
		{
			auto t = PeekNextToken();
			switch(t.Type)
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
		New(stmt, FunctionDef_t, FunctionDef, identifier, args, EmptyStmtBody, lineno, colno);
		HaveBodyNoEmpty(identifier, stmt->body);
		return stmt;
	}

	stmt_t Parser::TryClassDef()
	{
		Token t;
		TryToken(t, TokenType::KeywordClass);
		PinPosition(t, lineno, colno);
		TryIdentifier(t, identifier);
		TryToken(t, TokenType::SymbolParentesesOpen);
		New(stmt, ClassDef_t, ClassDef, identifier, EmptyExprList, EmptyStmtBody, lineno, colno);
		for(;;)
		{
			auto t = PeekNextToken();
			switch(t.Type)
			{
			case TokenType::SymbolComma:
				ReadNextToken();
				continue;
			case TokenType::Identifier: {
				TryIdentifier(t, identifier);
				stmt->bases.push_back(Name(identifier, expr_context_t::kLoad, t.Line, t.Column));
				continue;
			}
			default:
				break;
			}
			break;
		}
		TryToken(t, TokenType::SymbolParentesesClose);
		TryToken(t, TokenType::SymbolColon);
		TryToken(t, TokenType::NewLine);
		HaveBodyNoEmpty(identifier, stmt->body);
		return stmt;
	}

	stmt_t Parser::TryReturn()
	{
		Token t;
		expr_t value = nullptr;
		TryToken(t, TokenType::KeywordReturn);
		t = PeekNextToken();
		if(t.Type != TokenType::NewLine && t.Type != TokenType::End)
		{
			TryParsing(value, nullptr, TryExpression);
		}
		EndOrNewLine(t);
		return Return(value, t.Line, t.Column);
	}

	stmt_t Parser::TryDelete()
	{
		Token t;
		TryToken(t, TokenType::KeywordDel);
		TryParsingAuto(expr, nullptr, TryExpression);
		EndOrNewLine(t);
		return Delete(expr, t.Line, t.Column);
	}

	stmt_t Parser::TryAssign(expr_t target)
	{
		Token t;
		TryToken(t, TokenType::OpAssign);
		TryParsingAuto(expr, nullptr, TryExpression);
		EndOrNewLine(t);
		return Assign(target, expr, t.Line, t.Column);
	}

	stmt_t Parser::TryAugAssign(expr_t target)
	{
		Token t = ReadNextToken();
		TryParsingAuto(expr, nullptr, TryExpression);
		EndOrNewLine(t);

		switch(t.Type)
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
		TryParsingAuto(iter, nullptr, TryExpressionOperand);
		TryToken(t, TokenType::SymbolColon);
		New(stmt, For_t, For, target, iter, EmptyStmtBody, lineno, colno);
		t = PeekNextToken();
		switch(t.Type)
		{
		case TokenType::NewLine: {
			ReadNextToken();
			HaveBodyNoEmpty("for", stmt->body);
			break;
		}
		default: {
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
		for(;;)
		{
			auto t = PeekNextToken();
			switch(t.Type)
			{
			case TokenType::End:
				ExpectedToken(t, "in", nullptr);
			case TokenType::KeywordIn:
				if(names.size() == 0)
				{
					ParsingError(t, nullptr);
				}
				return names.size() == 1 ? names.front()
										 : Tuple(names, expr_context_t::kStore, t.Line, t.Column);
			case TokenType::SymbolComma:
				ReadNextToken();
				continue;
			case TokenType::Identifier: {
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
		TryParsingAuto(test, nullptr, TryExpression);
		TryToken(t, TokenType::SymbolParentesesClose);
		TryToken(t, TokenType::SymbolColon);
		New(stmt, While_t, While, test, EmptyStmtBody, lineno, colno);
		t = PeekNextToken();
		switch(t.Type)
		{
		case TokenType::NewLine: {
			ReadNextToken();
			HaveBodyNoEmpty("while", stmt->body);
		}
		default: {
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
		switch(t.Type)
		{
		case TokenType::KeywordIf: {
			TryParsingAuto(test, nullptr, TryExpression);
			TryToken(t, TokenType::SymbolColon);
			New(stmt, If_t, If, test, EmptyStmtBody, EmptyStmtBody, lineno, colno);
			t = PeekNextToken();
			switch(t.Type)
			{
			case TokenType::NewLine:
				ReadNextToken();
				HaveBodyNoEmpty("if", stmt->body);
				mState.IfStack.push(stmt);
				break;
			default:
				TryParsingAuto(body, nullptr, TryStatement);
				stmt->body.push_back(body);
				break;
			}
			return stmt;
		}
		case TokenType::KeywordElse: {
			MustNotHave(mState.IfStack.empty(), t, nullptr);
			TryToken(t, TokenType::SymbolColon);
			t = PeekNextToken();
			auto stmt = dynamic_cast<If_t>(mState.IfStack.top());
			switch(t.Type)
			{
			case TokenType::NewLine:
				ReadNextToken();
				HaveBodyNoEmpty("else", stmt->orelse);
				break;
			default:
				TryParsingAuto(body, nullptr, TryStatement);
				stmt->orelse.push_back(stmt);
				break;
			}
			return stmt;
		}
		case TokenType::KeywordElIf: {
			MustNotHave(mState.IfStack.empty(), t, nullptr);
			TryParsingAuto(test, nullptr, TryExpression);
			TryToken(t, TokenType::SymbolColon);
			New(orelse, If_t, If, test, EmptyStmtBody, EmptyStmtBody, lineno, colno);
			auto stmt = dynamic_cast<If_t>(mState.IfStack.top());
			mState.IfStack.pop();
			mState.IfStack.push(orelse);
			stmt->orelse.push_back(orelse);
			t = PeekNextToken();
			switch(t.Type)
			{
			case TokenType::NewLine:
				ReadNextToken();
				HaveBodyNoEmpty("elif", orelse->body);
				break;
			default:
				TryParsingAuto(body, nullptr, TryStatement);
				orelse->body.push_back(body);
				break;
			}
			return orelse;
		}
		default:
			break;
		}
		UnexpectedToken(t, nullptr);
	}

	stmt_t Parser::TryRaise()
	{
		Token t;
		TryToken(t, TokenType::KeywordRaise);
		TryParsingAuto(expr, nullptr, TryExpression);
		EndOrNewLine(t);
		return Raise(expr, t.Line, t.Column);
	}

	stmt_t Parser::TryTryExceptFinaly()
	{
		Token t = ReadNextToken();
		PinPosition(t, lineno, colno);
		switch(t.Type)
		{
		case TokenType::KeywordTry: {
			Token t;
			TryToken(t, TokenType::SymbolColon);
			EndOrNewLine(t);
			New(stmt, Try_t, Try, EmptyStmtBody, EmptyHandlers, EmptyStmtBody, lineno, colno);
			mState.TryStack.push(stmt);
			HaveBodyNoEmpty("try", stmt->body);
			return stmt;
		}
		case TokenType::KeywordExcept: {
			Token t;
			MustNotHave(mState.TryStack.empty(), t, nullptr);
			TryIdentifier(t, identifier);
			auto type = Name(identifier, expr_context_t::kLoad, t.Line, t.Column);
			TryToken(t, TokenType::KeywordAs);
			TryIdentifier(t, name);
			TryToken(t, TokenType::SymbolColon);
			EndOrNewLine(t);
			auto stmt = dynamic_cast<Try_t>(mState.TryStack.top());
			New(handler, ExceptHandler_t, ExceptHandler, type, name, EmptyStmtBody, lineno, colno);
			HaveBodyNoEmpty("except", handler->body);
			stmt->handlers.push_back(handler);
			return stmt;
		}
		case TokenType::KeywordFinally: {
			MustNotHave(mState.TryStack.empty(), t, nullptr);
			Token t;
			TryToken(t, TokenType::SymbolColon);
			EndOrNewLine(t);
			auto stmt = dynamic_cast<Try_t>(mState.TryStack.top());
			HaveBodyNoEmpty("finally", stmt->finalbody);
			return stmt;
		}
		default:
			break;
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
		TryParsingAuto(test, nullptr, TryExpression);
		t = PeekNextToken();
		if(t.Type == TokenType::SymbolComma)
		{
			ReadNextToken();
			TryParsing(msg, nullptr, TryConstant);
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
		for(;;)
		{
			auto t = PeekNextToken();
			switch(t.Type)
			{
			case TokenType::NewLine:
			case TokenType::End:
				ReadNextToken();
				if(name.size() != 0)
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
			case TokenType::KeywordAs: {
				ReadNextToken();
				TryIdentifier(t, identifier);
				asName = identifier;
				continue;
			}
			case TokenType::Identifier: {
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
		for(;;)
		{
			auto t = PeekNextToken();
			switch(t.Type)
			{
			case TokenType::NewLine:
			case TokenType::End:
				ReadNextToken();
				if(name.size() != 0)
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
			case TokenType::KeywordAs: {
				ReadNextToken();
				TryIdentifier(t, identifier);
				asName = identifier;
				continue;
			}
			case TokenType::Identifier: {
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
		for(;;)
		{
			auto t = PeekNextToken();
			switch(t.Type)
			{
			case TokenType::NewLine:
				ReadNextToken();
			case TokenType::End:
				MustHave(expr, t, nullptr);
				return Expr(expr, expr->lineno, expr->colno);
			case TokenType::Identifier:
				MustNotHave(expr, t, nullptr);
				TryParsing(expr, nullptr, TryName);
				t = PeekNextToken();
				switch(t.Type)
				{
				case TokenType::SymbolPeriod:
					TryParsing(expr, nullptr, TryAttribute, expr);
					continue;
				case TokenType::SymbolBracketOpen:
					TryParsing(expr, nullptr, TrySubscript, expr);
					continue;
				case TokenType::SymbolParentesesOpen:
					TryParsing(expr, nullptr, TryCall, expr);
					continue;
				default:
					continue;
				}
				continue;
			case TokenType::NumberFloat:
			case TokenType::NumberInteger:
			case TokenType::NumberHex:
			case TokenType::String:
				MustNotHave(expr, t, nullptr);
				TryParsing(expr, nullptr, TryConstant);
				continue;
			case TokenType::SymbolParentesesOpen:
				ReadNextToken();
				TryParsing(expr, nullptr, TryTuple, nullptr);
				TryToken(t, TokenType::SymbolParentesesClose);
				continue;
			case TokenType::SymbolComma:
				ReadNextToken();
				TryParsing(expr, nullptr, TryTuple, expr);
				continue;
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
				UnexpectedToken(t, nullptr);
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

		for(;;)
		{
			auto t = PeekNextToken();

			switch(t.Type)
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
		if(t.Type == TokenType::SymbolColon)
		{
			ReadNextToken();
			TryIdentifier(t, type);
			if(sTypesMap.find(type) == sTypesMap.end())
			{
				UnexpectedToken(t, nullptr);
			}
			return arg(identifier, sTypesMap.at(type), lineno, colno);
		}
		return arg(identifier, kind_t::kObject, lineno, colno);
	}

	expr_t Parser::TryExpression()
	{
		expr_t expr = nullptr;
		for(;;)
		{
			auto t = PeekNextToken();
			switch(t.Type)
			{
			case TokenType::Identifier:
			case TokenType::NumberFloat:
			case TokenType::NumberInteger:
			case TokenType::NumberHex:
			case TokenType::String:
			case TokenType::SymbolParentesesOpen:
			case TokenType::SymbolBracketOpen:
			case TokenType::SymbolBraceOpen:
				TryParsing(expr, nullptr, TryExpressionOperand);
				break;
			case TokenType::SymbolComma:
				TryParsing(expr, nullptr, TryTuple, expr);
				break;
			case TokenType::OpInv:
			case TokenType::OpAdd:
			case TokenType::OpSub:
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
			case TokenType::KeywordAnd:
			case TokenType::KeywordOr:
			case TokenType::KeywordNot:
			case TokenType::OpEqual:
			case TokenType::OpNotEqual:
			case TokenType::OpGreatEqual:
			case TokenType::OpLessEqual:
			case TokenType::OpGreat:
			case TokenType::OpLess:
			case TokenType::KeywordIs:
			case TokenType::KeywordIn:
				TryParsing(expr, nullptr, TryExpressionOperator, expr);
				continue;
			case TokenType::KeywordLambda:
				return TryLambda();
			default:
				if(!expr)
				{
					UnexpectedToken(t, nullptr);
				}
				return expr;
			}
		}
		return nullptr;
	}

	expr_t Parser::TryExpressionOperand()
	{
		auto t = PeekNextToken();
		std::string signal = "";
		for(;;)
		{
			switch(t.Type)
			{
			case TokenType::Identifier: {
				TryParsingAuto(operand, nullptr, TryName);
				auto t = PeekNextToken();
				switch(t.Type)
				{
				case TokenType::SymbolPeriod:
					TryParsing(operand, nullptr, TryAttribute, operand);
					return operand;
				case TokenType::SymbolBracketOpen:
					TryParsing(operand, nullptr, TrySubscript, operand);
					return operand;
				case TokenType::SymbolParentesesOpen:
					TryParsing(operand, nullptr, TryCall, operand);
					return operand;
				default:
					break;
				}
				return operand;
			}
			case TokenType::OpAdd:
			case TokenType::OpSub: {
				ReadNextToken();
				auto op = t;
				t = PeekNextToken();
				if(t.Type == TokenType::SymbolParentesesOpen)
				{
					TryToken(t, TokenType::SymbolParentesesOpen);
					TryParsingAuto(operand, nullptr, TryExpression);
					TryToken(t, TokenType::SymbolParentesesClose);
					return TryUnaryOp(op, operand);
				}
				signal = op.Value;
				continue;
			}
			case TokenType::NumberFloat:
			case TokenType::NumberInteger: {
				TryParsingAuto(operand, nullptr, TryConstant, signal);
				return operand;
			}
			case TokenType::NumberHex:
			case TokenType::String: {
				TryParsingAuto(operand, nullptr, TryConstant);
				return operand;
			}
			case TokenType::KeywordNone:
				ReadNextToken();
				return Name("None", expr_context_t::kLoad, t.Line, t.Column);
			case TokenType::KeywordTrue:
				ReadNextToken();
				return Name("True", expr_context_t::kLoad, t.Line, t.Column);
			case TokenType::KeywordFalse:
				ReadNextToken();
				return Name("False", expr_context_t::kLoad, t.Line, t.Column);
			case TokenType::SymbolParentesesOpen: {
				ReadNextToken();
				TryParsingAuto(operand, nullptr, TryExpression);
				TryToken(t, TokenType::SymbolParentesesClose);
				return operand;
			}
			case TokenType::SymbolBracketOpen: {
				ReadNextToken();
				TryParsingAuto(operand, nullptr, TryList);
				TryToken(t, TokenType::SymbolBracketClose);
				return operand;
			}
			case TokenType::SymbolBraceOpen: {
				ReadNextToken();
				TryParsingAuto(operand, nullptr, TryDict);
				TryToken(t, TokenType::SymbolBraceClose);
				return operand;
			}
			default:
				UnexpectedToken(t, nullptr);
			}
		}
		return nullptr;
	}

	expr_t Parser::TryExpressionOperator(expr_t left)
	{
		auto t = PeekNextToken();
		switch(t.Type)
		{
		case TokenType::OpInv: {
			MustNotHave(left, t, nullptr);
			ReadNextToken();
			TryParsingAuto(operand, nullptr, TryExpressionOperand);
			TryParsingAuto(op, nullptr, TryUnaryOp, t, operand);
			return op;
		}
		case TokenType::OpAdd:
		case TokenType::OpSub: {
			if(!left)
			{
				return TryExpressionOperand();
			}
			ReadNextToken();
			TryParsingAuto(right, nullptr, TryExpressionOperand);
			return TryBinOp(t, left, right);
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
		case TokenType::OpOr: {
			MustHave(left, t, nullptr);
			ReadNextToken();
			TryParsingAuto(right, nullptr, TryExpressionOperand);
			return TryBinOp(t, left, right);
		}
		case TokenType::KeywordAnd:
		case TokenType::KeywordOr: {
			MustHave(left, t, nullptr);
			ReadNextToken();
			TryParsingAuto(right, nullptr, TryExpressionOperand);
			return TryBoolOp(t, left, right);
		}
		case TokenType::KeywordNot: {
			ReadNextToken();
			if(!left)
			{
				TryParsingAuto(operand, nullptr, TryExpressionOperand);
				TryParsingAuto(op, nullptr, TryUnaryOp, t, operand);
				return op;
			}
			t = PeekNextToken();
			if(t.Type == TokenType::KeywordIn)
			{
				ReadNextToken();
				TryParsingAuto(right, nullptr, TryExpressionOperand);
				TryParsingAuto(op, nullptr, TryCompare, t, left, right);
				dynamic_cast<Compare_t>(op)->op = cmpop_t::kNotIn;
				return op;
			}
			TryParsingAuto(right, nullptr, TryExpressionOperand);
			return TryCompare(t, left, right);
		}
		case TokenType::KeywordIs: {
			MustHave(left, t, nullptr);
			ReadNextToken();
			t = PeekNextToken();
			if(t.Type == TokenType::KeywordNot)
			{
				ReadNextToken();
				TryParsingAuto(right, nullptr, TryExpressionOperand);
				TryParsingAuto(op, nullptr, TryCompare, t, left, right);
				dynamic_cast<Compare_t>(op)->op = cmpop_t::kIsNot;
				return op;
			}
			TryParsingAuto(right, nullptr, TryExpressionOperand);
			return TryCompare(t, left, right);
		}
		case TokenType::OpEqual:
		case TokenType::OpNotEqual:
		case TokenType::OpGreatEqual:
		case TokenType::OpLessEqual:
		case TokenType::OpGreat:
		case TokenType::OpLess:
		case TokenType::KeywordIn: {
			MustHave(left, t, nullptr);
			ReadNextToken();
			TryParsingAuto(right, nullptr, TryExpressionOperand);
			return TryCompare(t, left, right);
		}
		default:
			break;
		}
		UnexpectedToken(t, nullptr);
	}

	expr_t Parser::TryBoolOp(Token t, expr_t left, expr_t right)
	{
		PinPosition(t, lineno, colno);
		boolop_t op;
		switch(t.Type)
		{
		case TokenType::KeywordAnd:
			op = boolop_t::kAnd;
			break;
		case TokenType::KeywordOr:
			op = boolop_t::kOr;
			break;
		default:
			break;
		}
		return BoolOp(left, op, right, lineno, colno);
	}

	expr_t Parser::TryBinOp(Token t, expr_t left, expr_t right)
	{
		PinPosition(t, lineno, colno);
		operator_t op;
		switch(t.Type)
		{
		case TokenType::OpAdd:
			op = operator_t::kAdd;
			break;
		case TokenType::OpSub:
			op = operator_t::kSub;
			break;
		case TokenType::OpMul:
			op = operator_t::kMult;
			break;
		case TokenType::OpDiv:
			op = operator_t::kDiv;
			break;
		case TokenType::OpExp:
			op = operator_t::kPow;
			break;
		case TokenType::OpMod:
			op = operator_t::kMod;
			break;
		case TokenType::OpXor:
			op = operator_t::kBitXor;
			break;
		case TokenType::OpFloorDiv:
			op = operator_t::kFloorDiv;
			break;
		case TokenType::OpShiftLeft:
			op = operator_t::kLShift;
			break;
		case TokenType::OpShiftRight:
			op = operator_t::kRShift;
			break;
		case TokenType::OpAnd:
			op = operator_t::kBitAnd;
			break;
		case TokenType::OpOr:
			op = operator_t::kBitOr;
			break;
		default:
			UnexpectedToken(t, nullptr);
		}
		return BinOp(left, op, right, lineno, colno);
	}

	expr_t Parser::TryUnaryOp(Token t, expr_t operand)
	{
		PinPosition(t, lineno, colno);
		unaryop_t op;
		switch(t.Type)
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
		return UnaryOp(op, operand, lineno, colno);
	}

	expr_t Parser::TryCompare(Token t, expr_t left, expr_t right)
	{
		cmpop_t op;
		switch(t.Type)
		{
		case TokenType::OpEqual:
			op = cmpop_t::kEq;
			break;
		case TokenType::OpNotEqual:
			op = cmpop_t::kNotEq;
			break;
		case TokenType::OpGreatEqual:
			op = cmpop_t::kGtE;
			break;
		case TokenType::OpLessEqual:
			op = cmpop_t::kLtE;
			break;
		case TokenType::OpGreat:
			op = cmpop_t::kGt;
			break;
		case TokenType::OpLess:
			op = cmpop_t::kLt;
			break;
		case TokenType::KeywordIs:
			op = cmpop_t::kIs;
			break;
		case TokenType::KeywordIn:
			op = cmpop_t::kIn;
			break;
		default:
			UnexpectedToken(t, nullptr);
		}
		return Compare(left, op, right, left->lineno, left->colno);
	}

	expr_t Parser::TryLambda()
	{
		Token t;
		TryToken(t, TokenType::KeywordLambda);
		PinPosition(t, lineno, colno);
		TryParsingAuto(args, nullptr, TryLambdaArguments);
		TryToken(t, TokenType::SymbolColon);
		TryParsingAuto(body, nullptr, TryExpression);
		return Lambda(args, body, lineno, colno);
	}

	arguments_t Parser::TryLambdaArguments()
	{
		std::vector<arg_t> args;

		for(;;)
		{
			auto t = PeekNextToken();

			switch(t.Type)
			{
			case TokenType::SymbolColon:
				return arguments(args);
			case TokenType::SymbolComma:
				ReadNextToken();
				continue;
			case TokenType::Identifier: {
				TryIdentifier(t, identifier);
				args.push_back(arg(identifier, kind_t::kObject, t.Line, t.Column));
				break;
			}
			default:
				UnexpectedToken(t, nullptr);
			}
		}

		return arguments(args);
	}

	expr_t Parser::TryDict()
	{
		std::vector<expr_t> keys;
		std::vector<expr_t> values;
		bool nextIsKey = true;
		for(;;)
		{
			auto t = PeekNextToken();
			switch(t.Type)
			{
			case TokenType::NewLine:
			case TokenType::Identation:
				ReadNextToken();
				continue;
			case TokenType::SymbolComma:
				ReadNextToken();
				nextIsKey = true;
				continue;
			case TokenType::SymbolColon:
				ReadNextToken();
				nextIsKey = false;
				continue;
			case TokenType::KeywordNone: {
				ReadNextToken();
				auto expr = Name("None", expr_context_t::kLoad, t.Line, t.Column);
				if(nextIsKey)
				{
					keys.push_back(expr);
				}
				else
				{
					values.push_back(expr);
				}
				continue;
			}
			case TokenType::KeywordTrue: {
				ReadNextToken();
				auto expr = Name("True", expr_context_t::kLoad, t.Line, t.Column);
				if(nextIsKey)
				{
					keys.push_back(expr);
				}
				else
				{
					values.push_back(expr);
				}
				continue;
			}
			case TokenType::KeywordFalse: {
				ReadNextToken();
				auto expr = Name("False", expr_context_t::kLoad, t.Line, t.Column);
				if(nextIsKey)
				{
					keys.push_back(expr);
				}
				else
				{
					values.push_back(expr);
				}
				continue;
			}
			case TokenType::NumberFloat:
			case TokenType::NumberInteger:
			case TokenType::NumberHex:
			case TokenType::String: {
				TryParsingAuto(expr, nullptr, TryConstant);
				if(nextIsKey)
				{
					keys.push_back(expr);
				}
				else
				{
					values.push_back(expr);
				}
				continue;
			}
			case TokenType::Identifier: {
				TryIdentifier(t, identifier);
				auto expr = Name(identifier, expr_context_t::kLoad, t.Line, t.Column);
				if(nextIsKey)
				{
					keys.push_back(expr);
				}
				else
				{
					values.push_back(expr);
				}
				continue;
			}
			case TokenType::SymbolParentesesOpen: {
				if(nextIsKey)
				{
					UnexpectedToken(t, nullptr);
				}
				ReadNextToken();
				TryParsingAuto(tuple, nullptr, TryTuple, nullptr);
				TryToken(t, TokenType::SymbolParentesesClose);
				values.push_back(tuple);
				continue;
			}
			case TokenType::SymbolBracketOpen: {
				if(nextIsKey)
				{
					UnexpectedToken(t, nullptr);
				}
				ReadNextToken();
				TryParsingAuto(list, nullptr, TryList);
				TryToken(t, TokenType::SymbolBracketClose);
				values.push_back(list);
				continue;
			}
			case TokenType::SymbolBraceOpen: {
				if(nextIsKey)
				{
					UnexpectedToken(t, nullptr);
				}
				ReadNextToken();
				TryParsingAuto(dict, nullptr, TryDict);
				TryToken(t, TokenType::SymbolBraceClose);
				values.push_back(dict);
				continue;
			}
			default:
				if(keys.size() != values.size())
				{
					UnexpectedToken(t, nullptr);
				}
				return Dict(keys, values, t.Line, t.Column);
			}
		}

		return nullptr;
	}

	expr_t Parser::TryCall(expr_t func)
	{
		Token t;
		New(call, Call_t, Call, func, EmptyExprList, func->lineno, func->colno);
		TryToken(t, TokenType::SymbolParentesesOpen);
		TryCallArgs(call->args);
		TryToken(t, TokenType::SymbolParentesesClose);
		return call;
	}

	void Parser::TryCallArgs(std::vector<expr_t>& args)
	{
		Token t;
		for(;;)
		{
			auto t = PeekNextToken();
			switch(t.Type)
			{
			case TokenType::SymbolParentesesClose:
				return;
			case TokenType::SymbolComma:
				ReadNextToken();
				continue;
			default: {
				TryParsingAuto(arg, , TryCallArg);
				args.push_back(arg);
			}
			}
		}
	}

	expr_t Parser::TryCallArg()
	{
		expr_t expr = nullptr;
		for(;;)
		{
			auto t = PeekNextToken();
			switch(t.Type)
			{
			case TokenType::Identifier:
			case TokenType::NumberFloat:
			case TokenType::NumberInteger:
			case TokenType::NumberHex:
			case TokenType::String:
			case TokenType::SymbolParentesesOpen:
			case TokenType::SymbolBracketOpen:
			case TokenType::SymbolBraceOpen:
				TryParsing(expr, nullptr, TryCallArgOperand);
				break;
			case TokenType::OpInv:
			case TokenType::OpAdd:
			case TokenType::OpSub:
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
			case TokenType::KeywordAnd:
			case TokenType::KeywordOr:
			case TokenType::KeywordNot:
			case TokenType::OpEqual:
			case TokenType::OpNotEqual:
			case TokenType::OpGreatEqual:
			case TokenType::OpLessEqual:
			case TokenType::OpGreat:
			case TokenType::OpLess:
			case TokenType::KeywordIs:
			case TokenType::KeywordIn:
				TryParsing(expr, nullptr, TryCallArgOperator, expr);
				continue;
			default:
				if(!expr)
				{
					UnexpectedToken(t, nullptr);
				}
				return expr;
			}
		}
		return nullptr;
	}

	expr_t Parser::TryCallArgOperand()
	{
		std::string signal = "";
		for(;;)
		{
			auto t = PeekNextToken();
			switch(t.Type)
			{
			case TokenType::Identifier: {
				TryParsingAuto(operand, nullptr, TryName);
				auto t = PeekNextToken();
				switch(t.Type)
				{
				case TokenType::SymbolPeriod:
					TryParsing(operand, nullptr, TryAttribute, operand);
					return operand;
				case TokenType::SymbolBracketOpen:
					TryParsing(operand, nullptr, TrySubscript, operand);
					return operand;
				case TokenType::SymbolParentesesOpen:
					TryParsing(operand, nullptr, TryCall, operand);
					return operand;
				default:
					break;
				}
				return operand;
			}
			case TokenType::OpAdd:
			case TokenType::OpSub: {
				ReadNextToken();
				auto op = t;
				t = PeekNextToken();
				if(t.Type == TokenType::SymbolParentesesOpen)
				{
					TryToken(t, TokenType::SymbolParentesesOpen);
					TryParsingAuto(operand, nullptr, TryExpression);
					TryToken(t, TokenType::SymbolParentesesClose);
					return TryUnaryOp(op, operand);
				}
				signal = op.Value;
				continue;
			}
			case TokenType::NumberFloat:
			case TokenType::NumberInteger: {
				TryParsingAuto(operand, nullptr, TryConstant, signal);
				return operand;
			}
			case TokenType::NumberHex:
			case TokenType::String: {
				TryParsingAuto(operand, nullptr, TryConstant);
				return operand;
			}
			case TokenType::KeywordNone:
				ReadNextToken();
				return Name("None", expr_context_t::kLoad, t.Line, t.Column);
			case TokenType::KeywordTrue:
				ReadNextToken();
				return Name("True", expr_context_t::kLoad, t.Line, t.Column);
			case TokenType::KeywordFalse:
				ReadNextToken();
				return Name("False", expr_context_t::kLoad, t.Line, t.Column);
			case TokenType::SymbolParentesesOpen: {
				ReadNextToken();
				TryParsingAuto(operand, nullptr, TryExpression);
				TryToken(t, TokenType::SymbolParentesesClose);
				return operand;
			}
			case TokenType::SymbolBracketOpen: {
				ReadNextToken();
				TryParsingAuto(operand, nullptr, TryList);
				TryToken(t, TokenType::SymbolBracketClose);
				return operand;
			}
			case TokenType::SymbolBraceOpen: {
				ReadNextToken();
				TryParsingAuto(operand, nullptr, TryDict);
				TryToken(t, TokenType::SymbolBraceClose);
				return operand;
			}
			default:
				UnexpectedToken(t, nullptr);
			}
		}
	}

	expr_t Parser::TryCallArgOperator(expr_t left)
	{
		auto t = PeekNextToken();
		switch(t.Type)
		{
		case TokenType::OpInv: {
			MustNotHave(left, t, nullptr);
			ReadNextToken();
			TryParsingAuto(operand, nullptr, TryCallArgOperand);
			TryParsingAuto(op, nullptr, TryUnaryOp, t, operand);
			return op;
		}
		case TokenType::OpAdd:
		case TokenType::OpSub: {
			if(!left)
			{
				return TryCallArgOperand();
			}
			ReadNextToken();
			TryParsingAuto(right, nullptr, TryCallArgOperand);
			return TryBinOp(t, left, right);
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
		case TokenType::OpOr: {
			MustHave(left, t, nullptr);
			ReadNextToken();
			TryParsingAuto(right, nullptr, TryCallArgOperand);
			return TryBinOp(t, left, right);
		}
		case TokenType::KeywordAnd:
		case TokenType::KeywordOr: {
			MustHave(left, t, nullptr);
			ReadNextToken();
			TryParsingAuto(right, nullptr, TryCallArgOperand);
			return TryBoolOp(t, left, right);
		}
		case TokenType::KeywordNot: {
			ReadNextToken();
			if(!left)
			{
				TryParsingAuto(operand, nullptr, TryCallArgOperand);
				TryParsingAuto(op, nullptr, TryUnaryOp, t, operand);
				return op;
			}
			TryParsingAuto(right, nullptr, TryCallArgOperand);
			return TryCompare(t, left, right);
		}
		case TokenType::OpEqual:
		case TokenType::OpNotEqual:
		case TokenType::OpGreatEqual:
		case TokenType::OpLessEqual:
		case TokenType::OpGreat:
		case TokenType::OpLess:
		case TokenType::KeywordIs:
		case TokenType::KeywordIn: {
			MustHave(left, t, nullptr);
			ReadNextToken();
			TryParsingAuto(right, nullptr, TryCallArgOperand);
			return TryCompare(t, left, right);
		}
		default:
			break;
		}
		UnexpectedToken(t, nullptr);
	}

	expr_t Parser::TryFormattedValue()
	{
		return nullptr;
	}

	expr_t Parser::TryConstant(std::string signal)
	{
		auto t = ReadNextToken();
		switch(t.Type)
		{
		case TokenType::NumberFloat:
			return Constant(
				constant(std::stod(signal + t.Value)), kind_t::kFloat, t.Line, t.Column);
		case TokenType::NumberHex:
			return Constant(
				constant(std::stoi(signal + t.Value, 0, 16)), kind_t::kInt, t.Line, t.Column);
		case TokenType::NumberInteger:
			return Constant(constant(std::stoi(signal + t.Value)), kind_t::kInt, t.Line, t.Column);
		case TokenType::String:
			return Constant(constant(t.Value), kind_t::kString, t.Line, t.Column);
		default:
			ANTOMIC_ERROR(
				"Unexpected token on line {0}, column {1}: {2}", t.Line, t.Column, t.Value);
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
		switch(t.Type)
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
		switch(t.Type)
		{
		case TokenType::SymbolBracketOpen:
			return TrySubscript(Subscript(value, slice, expr_context_t::kStore, lineno, colno));
		default:
			return Subscript(value, slice, expr_context_t::kLoad, lineno, colno);
		}
	}

	expr_t Parser::TryName()
	{
		Token t;
		TryIdentifier(t, identifier);
		PinPosition(t, lineno, colno);
		t = PeekNextToken();
		switch(t.Type)
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
			return Name(identifier, expr_context_t::kStore, lineno, colno);
		default:
			return Name(identifier, expr_context_t::kLoad, lineno, colno);
		}
	}

	expr_t Parser::TryList()
	{
		std::vector<expr_t> eltsRead;
		for(;;)
		{
			auto t = PeekNextToken();
			switch(t.Type)
			{
			case TokenType::NewLine:
			case TokenType::Identation:
			case TokenType::SymbolComma:
				ReadNextToken();
				continue;
			case TokenType::KeywordNone:
				ReadNextToken();
				eltsRead.push_back(Name("None", expr_context_t::kLoad, t.Line, t.Column));
				continue;
			case TokenType::KeywordTrue:
				ReadNextToken();
				eltsRead.push_back(Name("True", expr_context_t::kLoad, t.Line, t.Column));
				continue;
			case TokenType::KeywordFalse:
				ReadNextToken();
				eltsRead.push_back(Name("False", expr_context_t::kLoad, t.Line, t.Column));
				continue;
			case TokenType::NumberFloat:
			case TokenType::NumberInteger:
			case TokenType::NumberHex:
			case TokenType::String: {
				TryParsingAuto(expr, nullptr, TryConstant);
				eltsRead.push_back(expr);
				continue;
			}
			case TokenType::Identifier: {
				TryIdentifier(t, identifier);
				eltsRead.push_back(Name(identifier, expr_context_t::kLoad, t.Line, t.Column));
				continue;
			}
			case TokenType::SymbolBracketOpen: {
				ReadNextToken();
				TryParsingAuto(list, nullptr, TryList);
				TryToken(t, TokenType::SymbolBracketClose);
				eltsRead.push_back(list);
				continue;
			}
			case TokenType::SymbolParentesesOpen: {
				ReadNextToken();
				TryParsingAuto(tuple, nullptr, TryTuple, nullptr);
				TryToken(t, TokenType::SymbolParentesesClose);
				eltsRead.push_back(tuple);
				continue;
			}
			case TokenType::SymbolBraceOpen: {
				ReadNextToken();
				TryParsingAuto(dict, nullptr, TryDict);
				TryToken(t, TokenType::SymbolBraceClose);
				eltsRead.push_back(dict);
				continue;
			}
			default:
				return List(eltsRead, expr_context_t::kLoad, t.Line, t.Column);
			}
		}

		return nullptr;
	}

	expr_t Parser::TryTuple(expr_t first)
	{
		std::vector<expr_t> eltsRead;
		if(first)
		{
			eltsRead.push_back(first);
		}
		for(;;)
		{
			auto t = PeekNextToken();
			switch(t.Type)
			{
			case TokenType::Identation:
			case TokenType::SymbolComma:
				ReadNextToken();
				continue;
			case TokenType::KeywordNone:
				ReadNextToken();
				eltsRead.push_back(Name("None", expr_context_t::kLoad, t.Line, t.Column));
				continue;
			case TokenType::KeywordTrue:
				ReadNextToken();
				eltsRead.push_back(Name("True", expr_context_t::kLoad, t.Line, t.Column));
				continue;
			case TokenType::KeywordFalse:
				ReadNextToken();
				eltsRead.push_back(Name("False", expr_context_t::kLoad, t.Line, t.Column));
				continue;
			case TokenType::NumberFloat:
			case TokenType::NumberInteger:
			case TokenType::NumberHex:
			case TokenType::String: {
				TryParsingAuto(expr, nullptr, TryConstant);
				eltsRead.push_back(expr);
			}
				continue;
			case TokenType::Identifier: {
				TryIdentifier(t, identifier);
				eltsRead.push_back(Name(identifier, expr_context_t::kLoad, t.Line, t.Column));
				continue;
			}
			case TokenType::SymbolParentesesOpen: {
				ReadNextToken();
				TryParsingAuto(tuple, nullptr, TryTuple, nullptr);
				TryToken(t, TokenType::SymbolParentesesClose);
				eltsRead.push_back(tuple);
				continue;
			}
			case TokenType::SymbolBracketOpen: {
				ReadNextToken();
				TryParsingAuto(list, nullptr, TryList);
				TryToken(t, TokenType::SymbolBracketClose);
				eltsRead.push_back(list);
				continue;
			}
			case TokenType::SymbolBraceOpen: {
				ReadNextToken();
				TryParsingAuto(dict, nullptr, TryDict);
				TryToken(t, TokenType::SymbolBraceClose);
				eltsRead.push_back(dict);
				continue;
			}
			default:
				return Tuple(eltsRead, expr_context_t::kLoad, t.Line, t.Column);
			}
		}

		return nullptr;
	}

	expr_t Parser::TryIndex()
	{
		expr_t index = nullptr;
		std::string signal = "";
		for(;;)
		{
			auto t = PeekNextToken();
			switch(t.Type)
			{
			case TokenType::SymbolBracketClose:
				MustHave(index, t, nullptr);
				return Index(index, index->lineno, index->colno);
			case TokenType::Identifier: {
				MustNotHave(index, t, nullptr);
				TryIdentifier(t, identifier);
				index = Name(identifier, expr_context_t::kLoad, t.Line, t.Column);
				continue;
			}
			case TokenType::OpSub:
			case TokenType::OpAdd: {
				ReadNextToken();
				signal = t.Value;
				continue;
			}
			case TokenType::NumberInteger:
			case TokenType::NumberFloat:
			case TokenType::NumberHex:
			case TokenType::String:
				MustNotHave(index, t, nullptr);
				TryParsing(index, nullptr, TryConstant, signal);
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
		expr_t lower = first;
		expr_t upper = nullptr;
		expr_t step = nullptr;
		int colon = 0;
		std::string signal = "";
		for(;;)
		{
			auto t = PeekNextToken();
			switch(t.Type)
			{
			case TokenType::SymbolBracketClose:
				if(!lower && !upper && !step)
				{
					UnexpectedToken(t, nullptr);
				}
				return Slice(lower, upper, step, t.Line, t.Column);
			case TokenType::SymbolColon:
				if(colon == 2)
				{
					UnexpectedToken(t, nullptr);
				}
				ReadNextToken();
				colon++;
				continue;
			case TokenType::OpSub:
			case TokenType::OpAdd: {
				ReadNextToken();
				signal = t.Value;
				continue;
			}
			case TokenType::NumberInteger: {
				if(colon == 1)
				{
					TryParsing(upper, nullptr, TryConstant, signal);
					continue;
				}
				if(colon == 2)
				{
					TryParsing(step, nullptr, TryConstant, signal);
					continue;
				}
			}
			default:
				UnexpectedToken(t, nullptr);
			}
		}
		return nullptr;
	}
} // namespace Antomic