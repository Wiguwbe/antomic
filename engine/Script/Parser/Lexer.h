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
#include "Script/Parser/Reader.h"

namespace Antomic
{
	enum class TokenType
	{
		Invalid,
		End,
		NewLine,
		Identation,

		Comment,
		Identifier,
		NumberFloat,
		NumberInteger,
		NumberHex,
		String,
		FormatedString,

		KeywordAnd,
		KeywordAs,
		KeywordAssert,
		KeywordBreak,
		KeywordClass,
		KeywordContinue,
		KeywordDef,
		KeywordDel,
		KeywordElIf,
		KeywordElse,
		KeywordExcept,
		KeywordExec,
		KeywordFalse,
		KeywordFinally,
		KeywordFor,
		KeywordFrom,
		KeywordIf,
		KeywordImport,
		KeywordIn,
		KeywordIs,
		KeywordLambda,
		KeywordNone,
		KeywordNot,
		KeywordOr,
		KeywordPass,
		KeywordRaise,
		KeywordReturn,
		KeywordTrue,
		KeywordTry,
		KeywordWhile,

		OpAdd,
		OpSub,
		OpMul,
		OpDiv,
		OpExp,
		OpMod,
		OpAnd,
		OpOr,
		OpXor,
		OpFloorDiv,
		OpInv,
		OpShiftLeft,
		OpShiftRight,
		OpEqual,
		OpNotEqual,
		OpGreatEqual,
		OpLessEqual,
		OpGreat,
		OpLess,
		OpAssign,
		OpAddAssign,
		OpSubAssign,
		OpMulAssign,
		OpDivAssign,
		OpFloorDivAssign,
		OpModAssign,
		OpAndAssign,
		OpOrAssign,
		OpXorAssign,
		OpShiftLeftAssign,
		OpShiftRightAssign,
		OpExpAssign,

		SymbolBraceOpen,
		SymbolBraceClose,
		SymbolComma,
		SymbolColon,
		SymbolPeriod,
		SymbolSemiColon,
		SymbolBracketOpen,
		SymbolBracketClose,
		SymbolParentesesOpen,
		SymbolParentesesClose,

		/* abstract, not returned by lexer */
		OpNotIn,
		OpIsNot,
		OpUnaryAdd,
		OpUnarySub,
		/* `OpInv` is unary as well */
	};

	struct Token
	{
		TokenType Type;
		std::string Value;
		uint32_t Line;
		uint32_t Column;

		Token()
			: Type(TokenType::Invalid)
			, Value("")
			, Line(0)
			, Column(0)
		{ }
		Token(TokenType type, const std::string& value)
			: Type(type)
			, Value(value)
			, Line(0)
			, Column(0)
		{ }
		Token(TokenType type, uint32_t line, uint32_t column, const std::string& value)
			: Type(type)
			, Value(value)
			, Line(line)
			, Column(column)
		{ }
	};

	struct LexerState
	{
		uint32_t CurrentLine;
		uint32_t CurrentColumn;
		Token CurrentToken;
		bool MultiLine;

		LexerState()
			: CurrentLine(1)
			, CurrentColumn(1)
			, CurrentToken({TokenType::Invalid, ""})
			, MultiLine(false)
		{ }
		LexerState(uint32_t line, uint32_t column, Token token)
			: CurrentLine(line)
			, CurrentColumn(column)
			, CurrentToken(token)
			, MultiLine(false)
		{ }
	};

	class Lexer
	{
	public:
		Lexer(const std::string& name);
		Lexer(const std::string& expression, const std::string& name);
		Lexer(const Ref<Reader> reader);
		~Lexer() = default;

		Token Read();
		Token Peek();
		void Unread(Token);

	private:
		Token ParseNext();
		void ProcessComment();
		void ProcessString();
		void ProcessOpenParenteses();
		void ProcessCloseParenteses();
		void ProcessOpenBrackets();
		void ProcessCloseBrackets();
		void ProcessOpenBraces();
		void ProcessCloseBraces();
		void ProcessAdd();
		void ProcessSub();
		void ProcessDiv();
		void ProcessMul();
		void ProcessPeriod();
		void ProcessComma();
		void ProcessColon();
		void ProcessSemicolon();
		void ProcessMod();
		void ProcessGreatThen();
		void ProcessLessThen();
		void ProcessAssign();
		void ProcessXor();
		void ProcessNot();
		void ProcessNotEqual();
		void ProcessAnd();
		void ProcessOr();
		void ProcessFormatedString();
		void ProcessDocString();
		void ProcessNumber();
		void ProcessDecimalNumber();
		void ProcessHexNumber();
		void ProcessName();

	private:
		inline void NextLine()
		{
			mState.CurrentLine++;
			mState.CurrentColumn = 1;
		}

		inline void StartToken()
		{
			mState.CurrentToken.Line = mState.CurrentLine;
			mState.CurrentToken.Column = mState.CurrentColumn;
		}

		inline void StartToken(TokenType type)
		{
			mState.CurrentToken.Type = type;
			mState.CurrentToken.Line = mState.CurrentLine;
			mState.CurrentToken.Column = mState.CurrentColumn;
		}

		inline char ReadNextChar()
		{
			char c = mReader->Read();
			mState.CurrentColumn++;
			return c;
		}

		inline void NewToken() { mState.CurrentToken = {TokenType::Invalid, ""}; }
		inline void EndToken(TokenType type) { mState.CurrentToken.Type = type; }
		inline char PeekNextChar() { return mReader->Peek(); }

		inline bool IsDigit(const char& c) { return c >= '0' && c <= '9'; }
		inline bool IsLetter(const char& c)
		{
			return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
		}
		inline bool IsAlphaNumeric(const char& c) { return IsDigit(c) || IsLetter(c); }
		inline bool IsHexadecimalDigit(const char& c)
		{
			return IsDigit(c) || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F');
		}
		inline bool IsWhiteSpace(const char& c) { return c == ' ' || c == '\n' || c == '\t'; }

	private:
		Ref<Reader> mReader = nullptr;
		LexerState mState;
		std::queue<Token> mQueue;
	};
} // namespace Antomic