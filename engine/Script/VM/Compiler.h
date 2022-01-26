/*
   Copyright 2022 Tiago Teixeira (tiago.t@sapo.pt)

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
#include "Script/Parser/Ast.h"
#include "Script/Parser/Parser.h"

namespace Antomic
{
	enum ConstantType
	{
		Name,
		String,
		Integer,
		Float,
		Code,

		Invalid
	};

	/*
        31 instructions, 5 bits
    */
	enum OPCode
	{
		LOAD = 0,
		LOAD_CONST,
		STORE,
		DEL,
		POP,
		DUP,
		CALL,
		MAKE_FUNC,
		MAKE_CLASS,
		RETURN,
		JMP,
		JT,
		JNT,
		ITER,
		NEXT,
		SLICE,
		TUPLE,
		LIST,
		DICT,
		GETATTR,
		SETATTR,
		DELATTR,
		GETSUB,
		SETSUB,
		DELSUB,
		IMP, // import
		RAISE,
		ARIT, // arithmetic takes the operation as index
		TRY,
		HANDLE,
		END_TRY,
	};

	/*
        TODO
        To make a direct correspondece between arithmetic and
        operations defined in `Ast.h`,
        the `ARIT` OpCode could be split into the different
        categories as defined in `Ast.h`
    */
	enum AritOp
	{
		/* similar to `operator_t` */
		ADD = 1,
		SUB,
		MUL,
		MMUL, // matrix mult
		DIV,
		MOD,
		EXP,
		SHL,
		SHR,
		OR,
		XOR,
		AND,
		FDIV, // floor div
		/* unary op, `unaryop_t` + 13 (FIXME) */
		INV,
		NEG,
		EQ,
		NEQ,
		GEQ,
		LEQ,
		GT,
		LT,
		IS,
		IN,
		BOOL_AND,
		BOOL_OR,
		BOOL_NOT
		// TODO implement augmented assign as opcodes?
	};

	struct CompilerConstant_
	{
		ConstantType type;
		union
		{
			std::string name;
			std::string string;
			int integer;
			float decimal;
			std::string code;
		};

		CompilerConstant_()
			: type(ConstantType::Invalid)
			, integer(-1)
		{ }

		CompilerConstant_(ConstantType type)
			: type(type)
			, integer(-1)
		{ }
	};
	typedef struct CompilerConstant_ CompilerConstant;

	struct loop
	{
		// instruction pointers for continues and breaks
		std::vector<int> continues;
		std::vector<int> breaks;
	};
	typedef struct loop loop_t;

	struct try_exc
	{
		int try_ip;
		// jumps to `finally`
		std::vector<int> jumps;
	};
	typedef struct try_exc try_exc_t;

	class CompilerVisitor : BaseVisitor
	{
	private:
		// list of for literals
		std::vector<CompilerConstant*> constants;

		// code stack in order to avoid passing the code struct around
		std::stack<CompilerConstant*> code_stack;

		// loop stack
		std::stack<loop_t*> loops;

		// try/exc stack
		std::stack<try_exc_t*> tries;

		void newLoop(void)
		{
			loop_t* loop = new loop_t();
			loops.push(loop);
		}
		void newBreak(void) { loops.top()->breaks.push_back(emitEmtpy()); }
		void newContinue(void) { loops.top()->continues.push_back(emitEmtpy()); }
		void endLoop(int continue_index, int break_index)
		{
			loop_t* loop = loops.top();
			for(int i = 0; i < loop->continues.size(); i++)
			{
				emit(JMP, continue_index, loop->continues[i]);
			}
			for(int i = 0; i < loop->breaks.size(); i++)
			{
				emit(JMP, break_index, loop->breaks[i]);
			}
			loops.pop();
			delete loop;
		}

		void newTry(void)
		{
			try_exc_t* t = new try_exc_t();
			t->try_ip = emitEmtpy();
			tries.push(t);
		}
		void tryJump(void) { tries.top()->jumps.push_back(emitEmtpy()); }
		void endTry(int exc_ip, int finally_ip)
		{
			try_exc_t* t = tries.top();
			emit(TRY, exc_ip, t->try_ip);
			for(int i = 0; i < t->jumps.size(); i++)
			{
				emit(JMP, finally_ip, t->jumps[i]);
			}
			emit(END_TRY);
		}

		int newCode(void)
		{
			// this should not be comparable, all should be unique
			CompilerConstant* cc = new CompilerConstant(ConstantType::Code);
			constants.push_back(cc);
			code_stack.push(cc);
			return constants.size() - 1;
		}
		void endCode(void) { code_stack.pop(); }
		int getIP(void)
		{
			// get current instruction pointer
			// instructions are 2 bytes
			return code_stack.top()->code.size() / 2;
		}
		void emit(OPCode opcode, int index = 0)
		{
			CompilerConstant* cc = code_stack.top();
			uint8_t a = (opcode << 3) & 0xff;
			a |= (index & 0x700) >> 8;
			uint8_t b = index & 0xff;
			cc->code.push_back(opcode);
			cc->code.push_back(index);
		}
		void emit(OPCode opcode, AritOp op) { emit(opcode, (int)op); }
		void emit(OPCode opcode, int index, int offset)
		{
			CompilerConstant* cc = code_stack.top();
			uint8_t a = (opcode << 3) & 0xff;
			a |= (index & 0x700) >> 8;
			uint8_t b = index & 0xff;
			cc->code[offset] = a;
			cc->code[offset + 1] = b;
		}
		int emitEmtpy(void)
		{
			// to later populate, useful for forward jumps
			CompilerConstant* cc = code_stack.top();
			int csize = cc->code.size();
			cc->code.resize(csize + 2);
			return csize;
		}
		int newName(std::string value)
		{
			CompilerConstant* cc = new CompilerConstant(ConstantType::Name);
			cc->name = value;
			constants.push_back(cc);
			return constants.size() - 1;
		}
		int newString(std::string value)
		{
			CompilerConstant* cc = new CompilerConstant(ConstantType::String);
			cc->string = value;
			constants.push_back(cc);
			return constants.size() - 1;
		}
		int newInteger(int value)
		{
			CompilerConstant* cc = new CompilerConstant(ConstantType::Integer);
			cc->integer = value;
			constants.push_back(cc);
			return constants.size() - 1;
		}
		int newFloat(float value)
		{
			CompilerConstant* cc = new CompilerConstant(ConstantType::Float);
			cc->decimal = value;
			constants.push_back(cc);
			return constants.size() - 1;
		}
		int findName(std::string name)
		{
			for(int i = 0; i < constants.size(); i++)
			{
				CompilerConstant* cc = constants[i];
				if(cc->type == ConstantType::Name && cc->name == name)
				{
					return i;
				}
			}
			return -1;
		}
		int findString(std::string value)
		{
			for(int i = 0; i < constants.size(); i++)
			{
				CompilerConstant* cc = constants[i];
				if(cc->type == ConstantType::String && cc->string == value)
				{
					return i;
				}
			}
			return -1;
		}
		int findInteger(int value)
		{
			for(int i = 0; i < constants.size(); i++)
			{
				CompilerConstant* cc = constants[i];
				if(cc->type == ConstantType::Integer && cc->integer == value)
				{
					return i;
				}
			}
			return -1;
		}
		int findFloat(float value)
		{
			for(int i = 0; i < constants.size(); i++)
			{
				CompilerConstant* cc = constants[i];
				if(cc->type == ConstantType::Integer && cc->decimal == value)
				{
					return i;
				}
			}
			return -1;
		}

	public:
		void writeToFile(std::string filename);

		std::any visitModule(Module_t node);
		std::any visitExpression(Expression_t node);
		std::any visitFunctionDef(FunctionDef_t node);
		std::any visitClassDef(ClassDef_t node);
		std::any visitReturn(Return_t node);
		std::any visitDelete(Delete_t node);
		std::any visitAssign(Assign_t node);
		std::any visitAugAssign(AugAssign_t node);
		std::any visitFor(For_t node);
		std::any visitWhile(While_t node);
		std::any visitIf(If_t node);
		std::any visitRaise(Raise_t node);
		std::any visitTry(Try_t node);
		std::any visitAssert(Assert_t node);
		std::any visitImport(Import_t node);
		std::any visitImportFrom(ImportFrom_t node);
		std::any visitExpr(Expr_t node);
		std::any visitPass(Pass_t node);
		std::any visitBreak(Break_t node);
		std::any visitContinue(Continue_t node);
		std::any visitBoolOp(BoolOp_t node);
		std::any visitBinOp(BinOp_t node);
		std::any visitUnaryOp(UnaryOp_t node);
		std::any visitLambda(Lambda_t node);
		std::any visitDict(Dict_t node);
		std::any visitCompare(Compare_t node);
		std::any visitCall(Call_t node);
		std::any visitFormattedValue(FormattedValue_t node);
		std::any visitConstant(Constant_t node);
		std::any visitAttribute(Attribute_t node);
		std::any visitSubscript(Subscript_t node);
		std::any visitIndex(Index_t node);
		std::any visitName(Name_t node);
		std::any visitList(List_t node);
		std::any visitTuple(Tuple_t node);
		std::any visitSlice(Slice_t node);
		//std::any visitExpr_Context(expr_context_t value);
		//std::any visitKind(kind_t value);
		// std::any visitBoolop(boolop_t value);
		// std::any visitOperator(operator_t value);
		// std::any visitUnaryop(unaryop_t value);
		// std::any visitCmpop(cmpop_t value);
		std::any visitExceptHandler(ExceptHandler_t node);
		std::any visitArguments(arguments_t node);
		std::any visitArg(arg_t node);
		std::any visitAlias(alias_t node);
	};
} // namespace Antomic