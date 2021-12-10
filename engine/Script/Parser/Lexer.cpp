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
#include "Script/Parser/Lexer.h"

static std::map<std::string, Antomic::TokenType> sKeywordsMaps{
	{"and", Antomic::TokenType::KeywordAnd},
	{"as", Antomic::TokenType::KeywordAs},
	{"assert", Antomic::TokenType::KeywordAssert},
	{"break", Antomic::TokenType::KeywordBreak},
	{"class", Antomic::TokenType::KeywordClass},
	{"continue", Antomic::TokenType::KeywordContinue},
	{"def", Antomic::TokenType::KeywordDef},
	{"del", Antomic::TokenType::KeywordDel},
	{"return", Antomic::TokenType::KeywordReturn},
	{"elif", Antomic::TokenType::KeywordElIf},
	{"else", Antomic::TokenType::KeywordElse},
	{"except", Antomic::TokenType::KeywordExcept},
	{"exec", Antomic::TokenType::KeywordExec},
	{"False", Antomic::TokenType::KeywordFalse},
	{"finally", Antomic::TokenType::KeywordFinally},
	{"for", Antomic::TokenType::KeywordFor},
	{"from", Antomic::TokenType::KeywordFrom},
	{"if", Antomic::TokenType::KeywordIf},
	{"import", Antomic::TokenType::KeywordImport},
	{"in", Antomic::TokenType::KeywordIn},
	{"is", Antomic::TokenType::KeywordIs},
	{"lambda", Antomic::TokenType::KeywordLambda},
	{"None", Antomic::TokenType::KeywordNone},
	{"not", Antomic::TokenType::KeywordNot},
	{"or", Antomic::TokenType::KeywordOr},
	{"pass", Antomic::TokenType::KeywordPass},
	{"raise", Antomic::TokenType::KeywordRaise},
	{"True", Antomic::TokenType::KeywordTrue},
	{"try", Antomic::TokenType::KeywordTry},
	{"while", Antomic::TokenType::KeywordWhile},
};

namespace Antomic
{
	Lexer::Lexer(const std::string& name)
	{
		mReader = Reader::FromFile(name);
		mState = {1, 1, {TokenType::Invalid, ""}};
	}

	Lexer::Lexer(const std::string& expression, const std::string& name)
	{
		mReader = Reader::FromString(expression, name);
		mState = {1, 1, {TokenType::Invalid, ""}};
	}

	Lexer::Lexer(const Ref<Reader> reader)
	{
		mReader = reader;
		mState = {1, 1, {TokenType::Invalid, ""}};
	}

	void Lexer::ProcessComment()
	{
		StartToken(TokenType::Comment);
		while(PeekNextChar() != '\n' && !mReader->IsEOF())
		{
			mState.CurrentToken.Value += ReadNextChar();
		}
		if(mReader->IsEOF())
		{
			return;
		}
	}

	void Lexer::ProcessString()
	{
		StartToken();

		auto start = ReadNextChar();
		/* 
            We if we find double single quotes in a row we might be dealing
            with docstrings
        */
		if(start == '\'' && PeekNextChar() == '\'')
		{
			ReadNextChar();

			if(mReader->IsEOF())
			{
				// Only one quote founded it was an empty string
				StartToken(TokenType::String);
				return;
			}

			if(PeekNextChar() != '\'')
			{
				// Only one quote founded it was an empty string
				StartToken(TokenType::String);
				return;
			}

			// A docstring found
			return ProcessDocString();
		}

		/*
            We are dealing with string literals
        */
		bool escapeNext = false;
		while(!mReader->IsEOF())
		{
			auto c = ReadNextChar();

			if(c == start)
			{
				// We found the end of our string
				EndToken(TokenType::String);
				return;
			}

			if(c == '\\' && !escapeNext)
			{
				escapeNext = true;
				continue;
			}

			if(c == '\n')
			{
				if(!escapeNext)
				{
					// It's not a multiline string therefore
					// the string is wrong
					EndToken(TokenType::Invalid);
					return;
				}
				NextLine();
			}

			if(escapeNext)
			{
				escapeNext = false;
			}

			mState.CurrentToken.Value += c;
		}
	}

	void Lexer::ProcessOpenParenteses()
	{
		StartToken();
		mState.CurrentToken.Value += ReadNextChar();
		EndToken(TokenType::SymbolParentesesOpen);
	}

	void Lexer::ProcessCloseParenteses()
	{
		StartToken();
		mState.CurrentToken.Value += ReadNextChar();
		EndToken(TokenType::SymbolParentesesClose);
	}

	void Lexer::ProcessOpenBrackets()
	{
		StartToken();
		mState.CurrentToken.Value += ReadNextChar();
		EndToken(TokenType::SymbolBracketOpen);
	}

	void Lexer::ProcessCloseBrackets()
	{
		StartToken();
		mState.CurrentToken.Value += ReadNextChar();
		EndToken(TokenType::SymbolBracketClose);
	}

	void Lexer::ProcessOpenBraces()
	{
		StartToken();
		mState.CurrentToken.Value += ReadNextChar();
		EndToken(TokenType::SymbolBraceOpen);
	}

	void Lexer::ProcessCloseBraces()
	{
		StartToken();
		mState.CurrentToken.Value += ReadNextChar();
		EndToken(TokenType::SymbolBraceClose);
	}

	void Lexer::ProcessAdd()
	{
		StartToken();
		mState.CurrentToken.Value += ReadNextChar();
		if(PeekNextChar() == '=')
		{
			mState.CurrentToken.Value += ReadNextChar();
			EndToken(TokenType::OpAddAssign);
			return;
		}
		EndToken(TokenType::OpAdd);
	}

	void Lexer::ProcessSub()
	{
		StartToken();
		mState.CurrentToken.Value += ReadNextChar();
		if(PeekNextChar() == '=')
		{
			mState.CurrentToken.Value += ReadNextChar();
			EndToken(TokenType::OpSubAssign);
			return;
		}
		EndToken(TokenType::OpSub);
	}

	void Lexer::ProcessDiv()
	{
		StartToken();
		mState.CurrentToken.Value += ReadNextChar();
		switch(PeekNextChar())
		{
		case '=':
			mState.CurrentToken.Value += ReadNextChar();
			EndToken(TokenType::OpDivAssign);
			break;
		case '/':
			mState.CurrentToken.Value += ReadNextChar();
			if(PeekNextChar() == '=')
			{
				mState.CurrentToken.Value += ReadNextChar();
				EndToken(TokenType::OpFloorDivAssign);
			}
			else
			{
				EndToken(TokenType::OpFloorDiv);
			}
			break;
		default:
			EndToken(TokenType::OpDiv);
		}
	}

	void Lexer::ProcessMul()
	{
		StartToken();
		mState.CurrentToken.Value += ReadNextChar();
		switch(PeekNextChar())
		{
		case '=':
			mState.CurrentToken.Value += ReadNextChar();
			EndToken(TokenType::OpMulAssign);
			break;
		case '*':
			mState.CurrentToken.Value += ReadNextChar();
			if(PeekNextChar() == '=')
			{
				mState.CurrentToken.Value += ReadNextChar();
				EndToken(TokenType::OpExpAssign);
			}
			else
			{
				EndToken(TokenType::OpExp);
			}
			break;
		default:
			EndToken(TokenType::OpMul);
			break;
		}
	}

	void Lexer::ProcessPeriod()
	{
		StartToken();
		mState.CurrentToken.Value += ReadNextChar();
		if(PeekNextChar() >= '0' && PeekNextChar() <= '9')
		{
			ProcessDecimalNumber();
			return;
		}
		EndToken(TokenType::SymbolPeriod);
	}

	void Lexer::ProcessComma()
	{
		StartToken();
		mState.CurrentToken.Value += ReadNextChar();
		EndToken(TokenType::SymbolComma);
	}

	void Lexer::ProcessColon()
	{
		StartToken();
		mState.CurrentToken.Value += ReadNextChar();
		EndToken(TokenType::SymbolColon);
	}

	void Lexer::ProcessSemicolon()
	{
		StartToken();
		mState.CurrentToken.Value += ReadNextChar();
		EndToken(TokenType::SymbolSemiColon);
	}

	void Lexer::ProcessMod()
	{
		StartToken();
		mState.CurrentToken.Value += ReadNextChar();
		if(PeekNextChar() == '=')
		{
			mState.CurrentToken.Value += ReadNextChar();
			EndToken(TokenType::OpModAssign);
			return;
		}
		EndToken(TokenType::OpMod);
	}

	void Lexer::ProcessGreatThen()
	{
		StartToken();
		mState.CurrentToken.Value += ReadNextChar();
		switch(PeekNextChar())
		{
		case '=':
			mState.CurrentToken.Value += ReadNextChar();
			EndToken(TokenType::OpGreatEqual);
			break;
		case '>':
			mState.CurrentToken.Value += ReadNextChar();
			if(PeekNextChar() == '=')
			{
				mState.CurrentToken.Value += ReadNextChar();
				EndToken(TokenType::OpShiftRightAssign);
			}
			else
			{
				EndToken(TokenType::OpShiftRight);
			}
			break;
		default:
			EndToken(TokenType::OpGreat);
			break;
		}
	}

	void Lexer::ProcessLessThen()
	{
		StartToken();
		mState.CurrentToken.Value += ReadNextChar();
		switch(PeekNextChar())
		{
		case '=':
			mState.CurrentToken.Value += ReadNextChar();
			EndToken(TokenType::OpLessEqual);
			break;
		case '<':
			mState.CurrentToken.Value += ReadNextChar();
			if(PeekNextChar() == '=')
			{
				mState.CurrentToken.Value += ReadNextChar();
				EndToken(TokenType::OpShiftLeftAssign);
			}
			else
			{
				EndToken(TokenType::OpShiftLeft);
			}
			break;
		default:
			EndToken(TokenType::OpLess);
			break;
		}
	}

	void Lexer::ProcessAssign()
	{
		StartToken();
		mState.CurrentToken.Value += ReadNextChar();
		if(PeekNextChar() == '=')
		{
			mState.CurrentToken.Value += ReadNextChar();
			EndToken(TokenType::OpEqual);
			return;
		}
		EndToken(TokenType::OpAssign);
	}

	void Lexer::ProcessXor()
	{
		StartToken();
		mState.CurrentToken.Value += ReadNextChar();
		if(PeekNextChar() == '=')
		{
			mState.CurrentToken.Value += ReadNextChar();
			EndToken(TokenType::OpXorAssign);
			return;
		}
		EndToken(TokenType::OpXor);
	}

	void Lexer::ProcessNot()
	{
		StartToken();
		mState.CurrentToken.Value += ReadNextChar();
		EndToken(TokenType::OpInv);
	}

	void Lexer::ProcessNotEqual()
	{
		StartToken();
		mState.CurrentToken.Value += ReadNextChar();
		if(PeekNextChar() != '=')
		{
			EndToken(TokenType::Invalid);
			return;
		}
		mState.CurrentToken.Value += ReadNextChar();
		EndToken(TokenType::OpNotEqual);
	}

	void Lexer::ProcessAnd()
	{
		StartToken();
		mState.CurrentToken.Value += ReadNextChar();
		if(PeekNextChar() == '=')
		{
			mState.CurrentToken.Value += ReadNextChar();
			EndToken(TokenType::OpAndAssign);
			return;
		}
		EndToken(TokenType::OpAnd);
	}

	void Lexer::ProcessOr()
	{
		StartToken();
		mState.CurrentToken.Value += ReadNextChar();
		if(PeekNextChar() == '=')
		{
			mState.CurrentToken.Value += ReadNextChar();
			EndToken(TokenType::OpOrAssign);
			return;
		}
		EndToken(TokenType::OpOr);
	}

	void Lexer::ProcessFormatedString()
	{
		StartToken();
		uint32_t column = mState.CurrentColumn;
		uint32_t line = mState.CurrentLine;
		ReadNextChar();
		if(PeekNextChar() == '"')
		{
			ProcessString();
			mState.CurrentToken.Column = column;
			mState.CurrentToken.Line = line;
			EndToken(TokenType::FormatedString);
			return;
		}

		// We migth hit a reserverd word
		mState.CurrentToken.Value += "f";
		ProcessName();
		mState.CurrentToken.Column = column;
		mState.CurrentToken.Line = line;
	}

	void Lexer::ProcessDocString() { }

	void Lexer::ProcessNumber()
	{
		StartToken();
		if(PeekNextChar() == '0')
		{
			mState.CurrentToken.Value += ReadNextChar();
			if(PeekNextChar() == 'x')
			{
				mState.CurrentToken.Value += ReadNextChar();
				ProcessHexNumber();
				return;
			}
		}

		bool decimal = false;
		for(;;)
		{
			auto c = PeekNextChar();

			if(!IsDigit(c) && c != '.')
			{
				if(IsLetter(c))
				{
					EndToken(TokenType::Invalid);
					return;
				}

				if(decimal)
				{
					EndToken(TokenType::NumberFloat);
					return;
				}

				EndToken(TokenType::NumberInteger);
				return;
			}

			if(c == '.')
			{
				mState.CurrentToken.Value += ReadNextChar();
				ProcessDecimalNumber();
				return;
			}

			mState.CurrentToken.Value += ReadNextChar();
		}
	}

	void Lexer::ProcessHexNumber()
	{
		for(;;)
		{
			auto c = PeekNextChar();
			if(!IsHexadecimalDigit(c))
			{
				if(IsLetter(c) || c == '.')
				{
					EndToken(TokenType::Invalid);
					return;
				}

				EndToken(TokenType::NumberHex);
				return;
			}

			mState.CurrentToken.Value += ReadNextChar();
		}
	}

	void Lexer::ProcessDecimalNumber()
	{
		for(;;)
		{
			auto c = PeekNextChar();
			if(!IsDigit(c))
			{

				if(IsLetter(c) || c == '.')
				{
					EndToken(TokenType::Invalid);
					return;
				}

				EndToken(TokenType::NumberFloat);
				return;
			}

			mState.CurrentToken.Value += ReadNextChar();
		}
	}

	void Lexer::ProcessName()
	{
		StartToken();
		for(;;)
		{
			auto c = PeekNextChar();
			if(!IsAlphaNumeric(c) && c != '_')
			{
				// Check for the name in the keywords map
				if(sKeywordsMaps.find(mState.CurrentToken.Value) != sKeywordsMaps.end())
				{
					EndToken(sKeywordsMaps[mState.CurrentToken.Value]);
					return;
				}

				// Not a keyworkd, must be an identifier
				EndToken(TokenType::Identifier);
				return;
			}
			mState.CurrentToken.Value += ReadNextChar();
		}
	}

	Token Lexer::Read()
	{
		if(!mQueue.empty())
		{
			auto token = mQueue.front();
			mQueue.pop();
			return token;
		}

		if(mReader->IsEOF())
		{
			return Token(TokenType::End, "");
		}

		auto t = ParseNext();
		mQueue.push(t);
		if(t.Type == TokenType::NewLine)
		{
			auto next = ParseNext();
			if(next.Type != TokenType::Identation)
			{
				mQueue.push({TokenType::Identation, ""});
			}
			mQueue.push(next);
		}

		return Read();
	}

	Token Lexer::Peek()
	{
		if(mQueue.empty())
		{
			if(mReader->IsEOF())
			{
				return Token(TokenType::End, "");
			}
			auto t = ParseNext();
			mQueue.push(t);
			if(t.Type == TokenType::NewLine)
			{
				auto next = ParseNext();
				if(next.Type != TokenType::Identation)
				{
					mQueue.push({TokenType::Identation, ""});
				}
				mQueue.push(next);
			}
		}

		return mQueue.front();
	}

	Token Lexer::ParseNext()
	{
		if(mReader->IsEOF())
		{
			return Token(TokenType::End, "");
		}

		NewToken();

		while(!mReader->IsEOF())
		{
			char c = PeekNextChar();

			switch(c)
			{
			case '\\':
				// Escape next new line
				ReadNextChar();
				if(PeekNextChar() != '\n')
				{
					StartToken(TokenType::Invalid);
					return mState.CurrentToken;
				}
				mState.MultiLine = true;
				ReadNextChar();
				NextLine();
				continue;
			case '#':
				ProcessComment();
				return mState.CurrentToken;
			case '\'':
				ProcessString();
				return mState.CurrentToken;
			case '\"':
				ProcessString();
				return mState.CurrentToken;
			case ' ':
				// comming from a multiline string, spaces are ignored
				if(mState.MultiLine || mState.CurrentColumn > 1)
				{
					while(PeekNextChar() == ' ' && !mReader->IsEOF())
					{
						ReadNextChar();
					}
					continue;
				}

				// Not a multiline string and we are in the beginning, spaces are indentation in python
				StartToken(TokenType::Identation);
				while(PeekNextChar() == ' ' && !mReader->IsEOF())
				{
					mState.CurrentToken.Value += ReadNextChar();
				}

				// Empty spaces with no text, skip it
				if(PeekNextChar() == '\n')
				{
					ReadNextChar();
					NextLine();
					NewToken();
					continue;
				}
				return mState.CurrentToken;

			case '\t':
				// comming from a multiline string, tabs are ignored
				if(mState.MultiLine || mState.CurrentColumn > 1)
				{
					while(PeekNextChar() == '\t' && !mReader->IsEOF())
					{
						ReadNextChar();
					}
					continue;
				}

				// Not a multiline string, tabs are indentation in python
				StartToken(TokenType::Identation);
				while(PeekNextChar() == '\t' && !mReader->IsEOF())
				{
					mState.CurrentToken.Value += ReadNextChar();
				}
				// Empty spaces with no text, skip it
				if(PeekNextChar() == '\n')
				{
					ReadNextChar();
					NextLine();
					NewToken();
					continue;
				}
				return mState.CurrentToken;
			case '\n':
				mState.MultiLine = false;
				if(mState.CurrentColumn > 1)
				{
					StartToken(TokenType::NewLine);
					ReadNextChar();
					NextLine();
					return mState.CurrentToken;
				}
				ReadNextChar();
				NextLine();
				continue;
			case '(':
				ProcessOpenParenteses();
				return mState.CurrentToken;
			case ')':
				ProcessCloseParenteses();
				return mState.CurrentToken;
			case '[':
				ProcessOpenBrackets();
				return mState.CurrentToken;
			case ']':
				ProcessCloseBrackets();
				return mState.CurrentToken;
			case '{':
				ProcessOpenBraces();
				return mState.CurrentToken;
			case '}':
				ProcessCloseBraces();
				return mState.CurrentToken;
			case '+':
				ProcessAdd();
				return mState.CurrentToken;
			case '-':
				ProcessSub();
				return mState.CurrentToken;
			case '/':
				ProcessDiv();
				return mState.CurrentToken;
			case '*':
				ProcessMul();
				return mState.CurrentToken;
			case '.':
				ProcessPeriod();
				return mState.CurrentToken;
			case ',':
				ProcessComma();
				return mState.CurrentToken;
			case ':':
				ProcessColon();
				return mState.CurrentToken;
			case ';':
				ProcessSemicolon();
				return mState.CurrentToken;
			case '%':
				ProcessMod();
				return mState.CurrentToken;
			case '>':
				ProcessGreatThen();
				return mState.CurrentToken;
			case '<':
				ProcessLessThen();
				return mState.CurrentToken;
			case '=':
				ProcessAssign();
				return mState.CurrentToken;
			case '!':
				ProcessNotEqual();
				return mState.CurrentToken;
			case '^':
				ProcessXor();
				return mState.CurrentToken;
			case '~':
				ProcessNot();
				return mState.CurrentToken;
			case '&':
				ProcessAnd();
				return mState.CurrentToken;
			case '|':
				ProcessOr();
				return mState.CurrentToken;
			case 'f':
				ProcessFormatedString();
				return mState.CurrentToken;
			default:
				break;
			}

			if(c >= '0' && c <= '9')
			{
				ProcessNumber();
				return mState.CurrentToken;
			}

			ProcessName();
			return mState.CurrentToken;
		}

		return mState.CurrentToken;
	}

} // namespace Antomic