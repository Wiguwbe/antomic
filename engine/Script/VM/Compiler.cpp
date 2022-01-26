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
#include "Compiler.h"

namespace Antomic
{
	std::any CompilerVisitor::visitModule(Module_t node)
	{
		int index = newCode();
		for(int i = 0; i < node->body.size(); i++)
		{
			node->body[i]->visit(*this);
		}
		endCode();
		return index;
	}

	std::any CompilerVisitor::visitExpression(Expression_t expr) { expr->body->visit(*this); }

	std::any CompilerVisitor::visitFunctionDef(FunctionDef_t fdef)
	{
		/*
            save code on literals,
            emit code to save function as name
        */
		// `MAKE_FUNC <arg_count>` takes <stack> code object and after the argument names
		// save on new code instance
		int index = newCode();
		for(int i = 0; i < fdef->body.size(); i++)
		{
			fdef->body[i]->visit(*this);
		}
		endCode();

		// emit code to save it

		// argument names (reverse order)
		arguments_t args = fdef->args;
		for(auto it = args->args.rbegin(); it != args->args.rend(); it++)
		{
			int name_index;
			arg_t arg = *it;
			if((name_index = findName(arg->arg)) == -1)
			{
				name_index = newName(arg->arg);
			}
			emit(LOAD_CONST, name_index);
		}

		emit(LOAD_CONST, index); // load code object
		int findex;
		if((findex = findName(fdef->name)) == -1)
		{
			findex = newName(fdef->name);
		}
		emit(MAKE_FUNC, args->args.size());
		// make function
		emit(STORE, findex); // store code as name
	}

	std::any CompilerVisitor::visitClassDef(ClassDef_t node)
	{
		int code_index = newCode();
		// code ...
		for(int i = 0; i < node->body.size(); i++)
		{
			node->body[i]->visit(*this);
		}
		// bases in reverse order
		for(auto it = node->bases.rbegin(); it != node->bases.rend(); it++)
		{
			expr_t expr = *it;
			expr->visit(*this);
		}
		emit(MAKE_CLASS, node->bases.size());
		endCode();
		int id_index;
		if((id_index = findName(node->name)) == -1)
		{
			id_index = newName(node->name);
		}
		// run code
		emit(LOAD_CONST, code_index);
		emit(CALL, 0);
		emit(STORE, id_index);
	}

	std::any CompilerVisitor::visitReturn(Return_t node)
	{
		// push value to stack (run expr)
		if(node->value != nullptr)
		{
			node->value->visit(*this);
		}
		else
		{
			int none_index;
			if((none_index = findName("None")) == -1)
			{
				none_index = newName("None");
			}
			emit(LOAD, none_index);
		}

		// exit
		emit(RETURN);

		// return value is on top of stack
	}

	std::any CompilerVisitor::visitDelete(Delete_t node)
	{
		// delsub or delattr ?
		// check based on `expr_context_t`
		node->target->visit(*this);
	}

	std::any CompilerVisitor::visitAssign(Assign_t node)
	{
		// run expr
		node->value->visit(*this);

		// save on target
		node->target->visit(*this);
	}

	std::any CompilerVisitor::visitAugAssign(AugAssign_t node)
	{
		// left <op> expr
		node->target->visit(*this); // FIXME context will be different
		node->value->visit(*this);
		emit(ARIT, (AritOp)node->op);

		// and save on variable
		node->target->visit(*this);
	}

	std::any CompilerVisitor::visitFor(For_t node)
	{
		// get expr
		node->iter->visit(*this);
		emit(ITER);
		int continue_ip = emitEmtpy(); // NEXT <else>
		newLoop();
		node->target->visit(*this);
		for(int i = 0; i < node->body.size(); i++)
		{
			node->body[i]->visit(*this);
		}
		int break_ip = getIP();
		emit(NEXT, break_ip, continue_ip);
		endLoop(continue_ip, break_ip);
	}

	std::any CompilerVisitor::visitWhile(While_t node)
	{
		// run expr, compare, jmp if false
		// run code, go back to comparison
		int cmp_index = getIP();
		newLoop();

		node->test->visit(*this);
		int false_i_index = emitEmtpy(); // populate when we have jump offset
		// body
		for(int i = 0; i < node->body.size(); i++)
		{
			node->body[i]->visit(*this);
		}
		// jmp back
		emit(JMP, cmp_index);

		// if false, jump to here (next instruction)
		emit(JNT, getIP(), false_i_index);

		endLoop(cmp_index, false_i_index);
	}

	std::any CompilerVisitor::visitIf(If_t node)
	{
		// test expr
		node->test->visit(*this);

		// jmp to else, if not true
		int false_i_index = emitEmtpy();

		// if TRUE body
		for(int i = 0; i < node->body.size(); i++)
		{
			node->body[i]->visit(*this);
		}
		// jump to after else
		if(node->orelse.size())
		{
			// there is an else
			// if true, jump to after (coming from TRUE)
			int true_i_index = emitEmtpy();
			// jump to here if false
			emit(JNT, getIP(), false_i_index);
			for(int i = 0; i < node->orelse.size(); i++)
			{
				node->orelse[i]->visit(*this);
			}
			// jump to here from TRUE
			emit(JMP, getIP(), true_i_index);
		}
		else
		{
			// no else body (ah!)
			emit(JNT, getIP(), false_i_index);
		}
	}

	std::any CompilerVisitor::visitRaise(Raise_t node)
	{
		node->exc->visit(*this);
		emit(RAISE);
	}

	std::any CompilerVisitor::visitTry(Try_t node)
	{
		newTry(); // emits 'TRY'
		for(auto it = node->body.begin(); it != node->body.end(); it++)
		{
			(*it)->visit(*this);
		}
		// exceptions handlers are here
		int exc_ip = getIP();
		for(auto it = node->handlers.begin(); it != node->handlers.end(); it++)
		{
			(*it)->visit(*this);
		}
		// finally is here
		int finally_ip = getIP();
		for(auto it = node->finalbody.begin(); it != node->finalbody.end(); it++)
		{
			(*it)->visit(*this);
		}
		endTry(exc_ip, finally_ip);
	}

	std::any CompilerVisitor::visitAssert(Assert_t node)
	{
		node->test->visit(*this);
		int true_i_index = emitEmtpy(); // jump later

		// inside FALSE body (raise assesrtion...)
		node->msg->visit(*this);
		int ae_index;
		if((ae_index = findName("AssertionError")) == -1)
		{
			ae_index = newName("AssertionError");
		}
		emit(LOAD_CONST, ae_index);
		emit(CALL, 1); // create exception
		emit(RAISE);
	}

	std::any CompilerVisitor::visitImport(Import_t node)
	{
		for(int i = 0; i < node->names.size(); i++)
		{
			alias_t alias = node->names[i];
			// import (goes to stack)
			int name_index;
			if((name_index = findName(alias->name)) == -1)
			{
				name_index = newName(alias->name);
			}
			emit(IMP, name_index);
			// save as name
			int as_index;
			if((as_index = findName(alias->asname)) == -1)
			{
				as_index = newName(alias->asname);
			}
			emit(STORE, as_index);
		}
	}

	std::any CompilerVisitor::visitImportFrom(ImportFrom_t node)
	{
		int modname_index;
		if((modname_index = findName(node->module)) == -1)
		{
			modname_index = newName(node->module);
		}
		emit(IMP, modname_index);
		for(int i = 0; i < node->names.size(); i++)
		{
			alias_t name = node->names[i];
			emit(DUP);
			int attr_index;
			if((attr_index = findName(name->name)) == -1)
			{
				attr_index = newName(name->name);
			}
			emit(GETATTR, attr_index);
			// save as
			int as_index;
			if((as_index = findName(name->asname)) == -1)
			{
				as_index = newName(name->asname);
			}
			emit(STORE, as_index);
		}
		// remove duplicated module from stack
		emit(POP);
	}

	std::any CompilerVisitor::visitExpr(Expr_t node)
	{
		node->value->visit(*this);
		// TODO if `expr` is not Assign/AugAssign, then it should POP result
	}

	std::any CompilerVisitor::visitPass(Pass_t node)
	{
		// simply syntax, nothing to add
	}

	std::any CompilerVisitor::visitBreak(Break_t node) { newBreak(); }

	std::any CompilerVisitor::visitContinue(Continue_t node) { newContinue(); }

	std::any CompilerVisitor::visitBoolOp(BoolOp_t node)
	{
		node->left->visit(*this);
		node->right->visit(*this);
		emit(ARIT, node->op == boolop_t::kAnd ? BOOL_AND : BOOL_OR);
	}

	std::any CompilerVisitor::visitBinOp(BinOp_t node)
	{
		node->left->visit(*this);
		node->right->visit(*this);
		// should have a direct correspondence between `operator_t` and `AritOp`
		emit(ARIT, (AritOp)node->op);
	}

	std::any CompilerVisitor::visitUnaryOp(UnaryOp_t node)
	{
		node->operand->visit(*this);
		switch(node->op)
		{
			case unaryop_t::kInvert: emit(ARIT, INV); break;
			case unaryop_t::kNot: emit(ARIT, BOOL_NOT); break;
			case unaryop_t::kUSub: emit(ARIT, NEG); break;
			case unaryop_t::kUAdd: /* nothing? */; break;
		}
	}

	std::any CompilerVisitor::visitLambda(Lambda_t node)
	{
		// basically a function with `return <expr>`
		int code_index = newCode();
		node->body->visit(*this);
		emit(RETURN);
		endCode();
		// arguments in reverse order
		for(auto it = node->args->args.rbegin(); it != node->args->args.rend(); it++)
		{
			int name_index;
			if((name_index = findName((*it)->arg)) == -1)
			{
				name_index = newName((*it)->arg);
			}
			emit(LOAD_CONST, name_index);
		}
		emit(MAKE_FUNC, node->args->args.size());
		emit(LOAD_CONST, code_index);
	}

	std::any CompilerVisitor::visitDict(Dict_t node)
	{
		// push values
		for(auto it = node->values.rbegin(); it != node->values.rend(); it++)
		{
			(*it)->visit(*this);
		}
		// push keys
		for(auto it = node->keys.rbegin(); it != node->keys.rend(); it++)
		{
			(*it)->visit(*this);
		}

		emit(DICT);
	}

	std::any CompilerVisitor::visitCompare(Compare_t node)
	{
		node->left->visit(*this);
		node->right->visit(*this);
		switch(node->op)
		{
			case cmpop_t::kEq: emit(ARIT, EQ); break;
			case cmpop_t::kNotEq: emit(ARIT, NEQ); break;
			case cmpop_t::kGtE: emit(ARIT, GEQ); break;
			case cmpop_t::kLtE: emit(ARIT, LEQ); break;
			case cmpop_t::kGt: emit(ARIT, GT); break;
			case cmpop_t::kLt: emit(ARIT, LT); break;
			case cmpop_t::kIs: emit(ARIT, IS); break;
			case cmpop_t::kIsNot:
				emit(ARIT, IS);
				emit(ARIT, BOOL_NOT);
				break;
			case cmpop_t::kIn: emit(ARIT, IN); break;
			case cmpop_t::kNotIn:
				emit(ARIT, IN);
				emit(ARIT, BOOL_NOT);
				break;
		}
	}

	std::any CompilerVisitor::visitCall(Call_t node)
	{
		// push args reverse order
		for(auto it = node->args.rbegin(); it != node->args.rend(); it++)
		{
			expr_t arg = *it;
			arg->visit(*this);
		}
		node->func->visit(*this);
		emit(CALL, node->args.size());
	}

	std::any CompilerVisitor::visitFormattedValue(FormattedValue_t node)
	{
		// TODO parser currently doesn't support FormatedString
	}

	std::any CompilerVisitor::visitConstant(Constant_t node)
	{
		int constant_index;
		switch(node->kind)
		{
			case kind_t::kFloat: {
				float d = std::any_cast<float>(node->value);
				if((constant_index = findFloat(d)) == -1)
				{
					constant_index = newFloat(d);
				}
				break;
			}
			case kind_t::kInt: {
				int d = std::any_cast<int>(node->value);
				if((constant_index = findInteger(d)) == -1)
				{
					constant_index = newInteger(d);
				}
				break;
			}
			case kind_t::kString: {
				std::string d = std::any_cast<std::string>(node->value);
				if((constant_index = findString(d)) == -1)
				{
					constant_index = newString(d);
				}
				break;
			}
			case kind_t::kObject: {
				// identifier/name
				std::string d = std::any_cast<std::string>(node->value);
				if((constant_index = findName(d)) == -1)
				{
					constant_index = newName(d);
				}
				break;
			}
		}

		// if it's a name, load from globals
		emit(node->kind == kind_t::kObject ? LOAD : LOAD_CONST, constant_index);
	}

	std::any CompilerVisitor::visitAttribute(Attribute_t node)
	{
		int name_index;
		if((name_index = findName(node->name)) == -1)
		{
			name_index = newName(node->name);
		}
		node->value->visit(*this);
		switch(node->ctx)
		{
			case expr_context_t::kLoad: emit(GETATTR, name_index); break;
			case expr_context_t::kStore: emit(SETATTR, name_index); break;
			case expr_context_t::kDel: emit(DELATTR, name_index); break;
		}
	}

	std::any CompilerVisitor::visitSubscript(Subscript_t node)
	{
		// value (if any) at TOP
		// push object
		node->value->visit(*this);
		// push index
		node->sliceRead->visit(*this);
		switch(node->ctx)
		{
			case expr_context_t::kLoad: emit(GETSUB); break;
			case expr_context_t::kStore: emit(SETSUB); break;
			case expr_context_t::kDel: emit(DELSUB); break;
		}
	}

	std::any CompilerVisitor::visitIndex(Index_t node) { node->value->visit(*this); }

	std::any CompilerVisitor::visitName(Name_t node)
	{
		int name_index;
		if((name_index = findName(node->idRead)) == -1)
		{
			name_index = newName(node->idRead);
		}
		switch(node->ctx)
		{
			case expr_context_t::kLoad: emit(LOAD, name_index); break;
			case expr_context_t::kStore: emit(STORE, name_index); break;
			case expr_context_t::kDel: emit(DEL, name_index); break;
		}
	}

	std::any CompilerVisitor::visitList(List_t node)
	{
		// should only have a `store` context
		for(auto it = node->eltsRead.rbegin(); it != node->eltsRead.rend(); it++)
		{
			(*it)->visit(*this);
		}
		emit(LIST, node->eltsRead.size());
	}

	std::any CompilerVisitor::visitTuple(Tuple_t node)
	{
		// two cases, store and load
		if(node->ctx == expr_context_t::kLoad)
		{
			// create tuple
			// store items in reverse order
			for(auto it = node->eltsRead.rbegin(); it != node->eltsRead.rend(); it++)
			{
				expr_t e = *it;
				e->visit(*this);
			}
			emit(TUPLE, node->eltsRead.size());
		}
		else if(node->ctx == expr_context_t::kStore)
		{
			// FIXME this is kinda hacky
			// `eltsRead` are names
			// tuple object should be at `<stack>`
			emit(ITER); // create iterator of tuple
			std::vector<int> nexts;
			for(int i = 0; i < node->eltsRead.size(); i++)
			{
				// get next item
				nexts.push_back(emitEmtpy());
				// store
				node->eltsRead[i]->visit(*this);
			}
			// TODO handle tuple length mismatch
			int next_index = getIP();
			for(int i = 0; i < nexts.size(); i++)
			{
				emit(NEXT, next_index, nexts[i]);
			}
		}
	}

	std::any CompilerVisitor::visitSlice(Slice_t node)
	{
		int count = 0;
		// reverse order
		if(node->lower != nullptr)
		{
			if(node->upper != nullptr)
			{
				if(node->step != nullptr)
				{
					count++;
					node->step->visit(*this);
				}
				count++;
				node->upper->visit(*this);
			}
			count++;
			node->lower->visit(*this);
		}

		emit(SLICE, count);
	}

	std::any CompilerVisitor::visitExceptHandler(ExceptHandler_t node)
	{
		// exception should be at <stack>
		emit(DUP); // gonna use it
		node->type->visit(*this); // get exception type
		emit(ARIT, IS); // FIXME exception type may be a tuple
		// if it is not (JNT to next handler)
		int ip = emitEmtpy();
		// if is same type, save to name
		if(!node->name.empty())
		{
			int name_index;
			if((name_index = findName(node->name)) == -1)
			{
				name_index = newName(node->name);
			}
			emit(STORE, name_index);
		}
		else
		{
			// not using it
			emit(POP);
		}
		for(int i = 0; i < node->body.size(); i++)
		{
			node->body[i]->visit(*this);
		}
		emit(HANDLE);
		tryJump();
		emit(JNT, getIP(), ip);
	}
} // namespace Antomic