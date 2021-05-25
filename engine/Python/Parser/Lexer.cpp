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
#include "Python/Parser/Lexer.h"

namespace Antomic
{
    Lexer::Lexer(const std::string &name)
    {
        mReader = Reader::FromFile(name);
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
        while (PeekNextChar() != '\n' && !mReader->IsEOF())
        {
            mState.CurrentToken.Value += ReadNextChar();
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
        if (start == '\'' && PeekNextChar() == '\'')
        {
            auto next = ReadNextChar();

            if (mReader->IsEOF())
            {
                // Only one quote founded it was an empty string
                StartToken(TokenType::String);
                return;
            }

            if (PeekNextChar() != '\'')
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
        while (!mReader->IsEOF())
        {
            auto c = ReadNextChar();

            if (c == start)
            {
                // We found the end of our string
                EndToken(TokenType::String);
                return;
            }

            if (c == '\\' && !escapeNext)
            {
                escapeNext = true;
                continue;
            }

            if (c == '\n')
            {
                if (!escapeNext)
                {
                    // It's not a multiline string therefore
                    // the string is wrong
                    EndToken(TokenType::Invalid);
                    return;
                }
                NextLine();
            }

            if (escapeNext)
            {
                escapeNext = false;
            }

            mState.CurrentToken.Value += c;
        }
    }

    void Lexer::ProcessOpenParenteses()
    {
        StartToken();
        ReadNextChar();
        EndToken(TokenType::SymbolParentesesOpen);
    }

    void Lexer::ProcessCloseParenteses()
    {
        StartToken();
        ReadNextChar();
        EndToken(TokenType::SymbolParentesesClose);
    }

    void Lexer::ProcessOpenBrackets()
    {
        StartToken();
        ReadNextChar();
        EndToken(TokenType::SymbolBracketOpen);
    }

    void Lexer::ProcessCloseBrackets()
    {
        StartToken();
        ReadNextChar();
        EndToken(TokenType::SymbolBracketClose);
    }

    void Lexer::ProcessOpenBraces()
    {
        StartToken();
        ReadNextChar();
        EndToken(TokenType::SymbolBraceOpen);
    }

    void Lexer::ProcessCloseBraces()
    {
        StartToken();
        ReadNextChar();
        EndToken(TokenType::SymbolBraceClose);
    }

    void Lexer::ProcessAdd()
    {
        StartToken();
        ReadNextChar();
        if (PeekNextChar() == '=')
        {
            ReadNextChar();
            EndToken(TokenType::OpAddAssign);
            return;
        }
        EndToken(TokenType::OpAdd);
    }

    void Lexer::ProcessSub()
    {
        StartToken();
        ReadNextChar();
        if (PeekNextChar() == '=')
        {
            ReadNextChar();
            EndToken(TokenType::OpSubAssign);
            return;
        }
        EndToken(TokenType::OpSub);
    }

    void Lexer::ProcessDiv()
    {
        StartToken();
        ReadNextChar();
        switch (PeekNextChar())
        {
        case '=':
            ReadNextChar();
            EndToken(TokenType::OpDivAssign);
            break;
        case '/':
            ReadNextChar();
            if (PeekNextChar() == '=')
            {
                ReadNextChar();
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
        ReadNextChar();
        switch (PeekNextChar())
        {
        case '=':
            ReadNextChar();
            EndToken(TokenType::OpMulAssign);
            break;
        case '*':
            ReadNextChar();
            if (PeekNextChar() == '=')
            {
                ReadNextChar();
                EndToken(TokenType::OpExpAssign);
            }
            else
            {
                EndToken(TokenType::OpExp);
            }
        default:
            EndToken(TokenType::OpMul);
            break;
        }
    }

    void Lexer::ProcessPeriod()
    {
        StartToken();
        ReadNextChar();
        EndToken(TokenType::SymbolPeriod);
    }

    void Lexer::ProcessComma()
    {
        StartToken();
        ReadNextChar();
        if (PeekNextChar() >= '0' && PeekNextChar() <= '9')
        {
            ProcessDecimalNumber();
            return;
        }
        EndToken(TokenType::SymbolComma);
    }

    void Lexer::ProcessColon()
    {
        StartToken();
        ReadNextChar();
        EndToken(TokenType::SymbolColon);
    }

    void Lexer::ProcessSemicolon()
    {
        StartToken();
        ReadNextChar();
        EndToken(TokenType::SymbolSemiColon);
    }

    void Lexer::ProcessMod()
    {
        StartToken();
        ReadNextChar();
        if (PeekNextChar() == '=')
        {
            ReadNextChar();
            EndToken(TokenType::OpModAssign);
            return;
        }
        EndToken(TokenType::OpAdd);
    }

    void Lexer::ProcessAt()
    {
        StartToken();
        ReadNextChar();
        EndToken(TokenType::SymbolAt);
    }

    void Lexer::ProcessGreatThen()
    {
        StartToken();
        ReadNextChar();
        switch (PeekNextChar())
        {
        case '=':
            ReadNextChar();
            EndToken(TokenType::OpGreatEqual);
            break;
        case '>':
            ReadNextChar();
            if (PeekNextChar() == '=')
            {
                ReadNextChar();
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
        ReadNextChar();
        switch (PeekNextChar())
        {
        case '=':
            ReadNextChar();
            EndToken(TokenType::OpLessEqual);
            break;
        case '<':
            ReadNextChar();
            if (PeekNextChar() == '=')
            {
                ReadNextChar();
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
        ReadNextChar();
        if (PeekNextChar() == '=')
        {
            ReadNextChar();
            EndToken(TokenType::OpEqual);
            return;
        }
        EndToken(TokenType::OpAssign);
    }

    void Lexer::ProcessXor()
    {
        StartToken();
        ReadNextChar();
        if (PeekNextChar() == '=')
        {
            ReadNextChar();
            EndToken(TokenType::OpXorAssign);
            return;
        }
        EndToken(TokenType::OpXor);
    }

    void Lexer::ProcessNot()
    {
        StartToken();
        auto c = ReadNextChar();
        EndToken(TokenType::OpInv);
    }

    void Lexer::ProcessNotEqual()
    {
        StartToken();
        auto c = ReadNextChar();
        if (PeekNextChar() != '=')
        {
            EndToken(TokenType::Invalid);
            return;
        }
        EndToken(TokenType::OpNotEqual);
    }

    void Lexer::ProcessAnd()
    {
        StartToken();
        ReadNextChar();
        if (PeekNextChar() == '=')
        {
            ReadNextChar();
            EndToken(TokenType::OpAndAssign);
            return;
        }
        EndToken(TokenType::OpAnd);
    }

    void Lexer::ProcessOr()
    {
        StartToken();
        ReadNextChar();
        if (PeekNextChar() == '=')
        {
            ReadNextChar();
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
        if (PeekNextChar() == '"')
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

    void Lexer::ProcessDocString()
    {
    }

    void Lexer::ProcessNumber()
    {
        StartToken();
        if (PeekNextChar() == '0')
        {
            mState.CurrentToken.Value += ReadNextChar();
            if (PeekNextChar() == 'x')
            {
                mState.CurrentToken.Value += ReadNextChar();
                ProcessHexNumber();
                return;
            }
        }

        bool decimal = false;
        for (;;)
        {
            auto c = PeekNextChar();

            if (c < '0' || c > '9')
            {
                if (decimal)
                {
                    EndToken(TokenType::NumberFloat);
                    return;
                }

                EndToken(TokenType::NumberInteger);
                return;
            }

            if (c == '.')
            {
                if (decimal)
                {
                    EndToken(TokenType::Invalid);
                    return;
                }

                decimal = true;
            }

            mState.CurrentToken.Value += ReadNextChar();
        }
    }

    void Lexer::ProcessHexNumber()
    {
        for (;;)
        {
            auto c = PeekNextChar();
            if ((c < '0' || c > '9') && (c < 'a' || c > 'z') && (c < 'A' || c > 'Z'))
            {
                EndToken(TokenType::NumberHex);
                return;
            }

            if (c == '.')
            {
                EndToken(TokenType::Invalid);
                return;
            }

            mState.CurrentToken.Value += ReadNextChar();
        }
    }

    void Lexer::ProcessDecimalNumber()
    {
        for (;;)
        {
            auto c = PeekNextChar();
            if (c < '0' || c > '9')
            {
                EndToken(TokenType::NumberFloat);
                return;
            }

            if (c == '.')
            {
                EndToken(TokenType::Invalid);
                return;
            }

            mState.CurrentToken.Value += ReadNextChar();
        }
    }

    void Lexer::ProcessName()
    {
        StartToken();
        for (;;)
        {
            auto c = PeekNextChar();
            if ((c < 'a' || c > 'z') && (c < 'A' || c > 'Z') && c != '_' && (c < '0' || c > '9'))
            {
                if (mState.CurrentToken.Value.compare("and") == 0)
                {
                    EndToken(TokenType::KeywordAnd);
                    return;
                }

                if (mState.CurrentToken.Value.compare("as") == 0)
                {
                    EndToken(TokenType::KeywordAs);
                    return;
                }

                if (mState.CurrentToken.Value.compare("assert") == 0)
                {
                    EndToken(TokenType::KeywordAssert);
                    return;
                }

                if (mState.CurrentToken.Value.compare("break") == 0)
                {
                    EndToken(TokenType::KeywordBreak);
                    return;
                }

                if (mState.CurrentToken.Value.compare("class") == 0)
                {
                    EndToken(TokenType::KeywordClass);
                    return;
                }

                if (mState.CurrentToken.Value.compare("continue") == 0)
                {
                    EndToken(TokenType::KeywordContinue);
                    return;
                }

                if (mState.CurrentToken.Value.compare("def") == 0)
                {
                    EndToken(TokenType::KeywordDef);
                    return;
                }

                if (mState.CurrentToken.Value.compare("del") == 0)
                {
                    EndToken(TokenType::KeywordDel);
                    return;
                }

                if (mState.CurrentToken.Value.compare("elif") == 0)
                {
                    EndToken(TokenType::KeywordElIf);
                    return;
                }

                if (mState.CurrentToken.Value.compare("else") == 0)
                {
                    EndToken(TokenType::KeywordElse);
                    return;
                }

                if (mState.CurrentToken.Value.compare("except") == 0)
                {
                    EndToken(TokenType::KeywordExcept);
                    return;
                }

                if (mState.CurrentToken.Value.compare("exec") == 0)
                {
                    EndToken(TokenType::KeywordExec);
                    return;
                }

                if (mState.CurrentToken.Value.compare("False") == 0)
                {
                    EndToken(TokenType::KeywordFalse);
                    return;
                }

                if (mState.CurrentToken.Value.compare("finally") == 0)
                {
                    EndToken(TokenType::KeywordFinally);
                    return;
                }

                if (mState.CurrentToken.Value.compare("for") == 0)
                {
                    EndToken(TokenType::KeywordFor);
                    return;
                }

                if (mState.CurrentToken.Value.compare("from") == 0)
                {
                    EndToken(TokenType::KeywordFrom);
                    return;
                }

                if (mState.CurrentToken.Value.compare("global") == 0)
                {
                    EndToken(TokenType::KeywordGlobal);
                    return;
                }

                if (mState.CurrentToken.Value.compare("if") == 0)
                {
                    EndToken(TokenType::KeywordIf);
                    return;
                }

                if (mState.CurrentToken.Value.compare("import") == 0)
                {
                    EndToken(TokenType::KeywordImport);
                    return;
                }

                if (mState.CurrentToken.Value.compare("in") == 0)
                {
                    EndToken(TokenType::KeywordIn);
                    return;
                }

                if (mState.CurrentToken.Value.compare("is") == 0)
                {
                    EndToken(TokenType::KeywordIs);
                    return;
                }

                if (mState.CurrentToken.Value.compare("lambda") == 0)
                {
                    EndToken(TokenType::KeywordLambda);
                    return;
                }

                if (mState.CurrentToken.Value.compare("None") == 0)
                {
                    EndToken(TokenType::KeywordNone);
                    return;
                }

                if (mState.CurrentToken.Value.compare("not") == 0)
                {
                    EndToken(TokenType::KeywordNot);
                    return;
                }

                if (mState.CurrentToken.Value.compare("or") == 0)
                {
                    EndToken(TokenType::KeywordOr);
                    return;
                }

                if (mState.CurrentToken.Value.compare("pass") == 0)
                {
                    EndToken(TokenType::KeywordPass);
                    return;
                }

                if (mState.CurrentToken.Value.compare("raise") == 0)
                {
                    EndToken(TokenType::KeywordRaise);
                    return;
                }

                if (mState.CurrentToken.Value.compare("def") == 0)
                {
                    EndToken(TokenType::KeywordReturn);
                    return;
                }

                if (mState.CurrentToken.Value.compare("True") == 0)
                {
                    EndToken(TokenType::KeywordTrue);
                    return;
                }

                if (mState.CurrentToken.Value.compare("try") == 0)
                {
                    EndToken(TokenType::KeywordTry);
                    return;
                }

                if (mState.CurrentToken.Value.compare("while") == 0)
                {
                    EndToken(TokenType::KeywordWhile);
                    return;
                }

                if (mState.CurrentToken.Value.compare("with") == 0)
                {
                    EndToken(TokenType::KeywordWith);
                    return;
                }

                EndToken(TokenType::Identifier);
                return;
            }
            mState.CurrentToken.Value += ReadNextChar();
        }
    }

    Token Lexer::Next()
    {
        if (mReader->IsEOF())
        {
            return Token(TokenType::End, "");
        }

        NewToken();

        while (!mReader->IsEOF())
        {
            char c = PeekNextChar();

            switch (c)
            {
            case '\\':
                // Escape next new line
                ReadNextChar();
                if (PeekNextChar() != '\n')
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
                if (mState.MultiLine || mState.CurrentColumn > 1)
                {
                    while (PeekNextChar() == ' ' && !mReader->IsEOF())
                    {
                        ReadNextChar();
                    }
                    continue;
                }

                // Not a multiline string and we are in the beginning, spaces are indentation in python
                StartToken(TokenType::Identation);
                while (PeekNextChar() == ' ' && !mReader->IsEOF())
                {
                    mState.CurrentToken.Value += ReadNextChar();
                }
                return mState.CurrentToken;

            case '\t':
                // comming from a multiline string, tabs are ignored
                if (mState.MultiLine || mState.CurrentColumn > 1)
                {
                    while (PeekNextChar() == '\t' && !mReader->IsEOF())
                    {
                        ReadNextChar();
                    }
                    continue;
                }

                // Not a multiline string, tabs are indentation in python
                StartToken(TokenType::Identation);
                while (PeekNextChar() == '\t' && !mReader->IsEOF())
                {
                    mState.CurrentToken.Value += ReadNextChar();
                }
                return mState.CurrentToken;
            case '\n':
                mState.MultiLine = false;
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
            case '@':
                ProcessAt();
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

            if (c >= '0' && c <= '9')
            {
                ProcessNumber();
                return mState.CurrentToken;
            }

            ProcessName();
            return mState.CurrentToken;
        }

        return mState.CurrentToken;
    }
}