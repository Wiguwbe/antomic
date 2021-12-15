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
#include "ScriptEngine.h"
#include "Parser/Parser.h"
#include "Script/Types/Types.h"

namespace Antomic
{
	ScriptEngine::ScriptEngine() { mScope = CreateRef<ScriptScope>(); }

	ScriptEngine::~ScriptEngine()
	{
		if(mScope != nullptr)
			mScope->CleanUp();
	}

	Ref<ScriptObject> ScriptEngine::Eval(const std::string& expr)
	{
		Parser parser;
		auto e = parser.FromExpression(expr);

		return Eval(e);
	}

	Ref<ScriptObject> ScriptEngine::Eval(mod_t node)
	{
		switch(node->mod_kind)
		{
			case Antomic::mod_::ModKind::kModule: {
				// auto mod = dynamic_cast<Antomic::Module_t>(node);
				// TODO: implement module
				return CreateRef<None>();
			}
			case Antomic::mod_::ModKind::kExpression: {
				auto expr = dynamic_cast<Expression_t>(node);
				return Eval(expr->body);
			}
		}
		return CreateRef<None>();
	}

	Ref<ScriptObject> ScriptEngine::Eval(stmt_t node)
	{
		switch(node->stmt_kind)
		{
			case Antomic::stmt_::StmtKind::kAssert: {
				return CreateRef<None>();
			}
			case Antomic::stmt_::StmtKind::kAssign: {
				return CreateRef<None>();
			}
			case Antomic::stmt_::StmtKind::kAugAssign: {
				return CreateRef<None>();
			}
			case Antomic::stmt_::StmtKind::kBreak: return CreateRef<None>();
			case Antomic::stmt_::StmtKind::kClassDef: {
				return CreateRef<None>();
			}
			case Antomic::stmt_::StmtKind::kContinue: return CreateRef<None>();
			case Antomic::stmt_::StmtKind::kDelete: {
				return CreateRef<None>();
			}
			case Antomic::stmt_::StmtKind::kExpr: {
				return CreateRef<None>();
			}
			case Antomic::stmt_::StmtKind::kFor: {
				return CreateRef<None>();
			}
			case Antomic::stmt_::StmtKind::kFunctionDef: {
				return CreateRef<None>();
			}
			case Antomic::stmt_::StmtKind::kIf: {
				return CreateRef<None>();
			}
			case Antomic::stmt_::StmtKind::kImport: {
				return CreateRef<None>();
			}
			case Antomic::stmt_::StmtKind::kImportFrom: {
				return CreateRef<None>();
			}
			case Antomic::stmt_::StmtKind::kPass: {
				return CreateRef<None>();
			}
			case Antomic::stmt_::StmtKind::kRaise: {
				return CreateRef<None>();
			}
			case Antomic::stmt_::StmtKind::kReturn: {
				return CreateRef<None>();
			}
			case Antomic::stmt_::StmtKind::kTry: {
				return CreateRef<None>();
			}
			case Antomic::stmt_::StmtKind::kWhile: {
				return CreateRef<None>();
			}
		}
		return CreateRef<None>();
	}

	Ref<ScriptObject> ScriptEngine::Eval(expr_t node)
	{
		if(!node)
		{
			return CreateRef<None>();
		}

		switch(node->expr_kind)
		{
			case Antomic::expr_::ExprKind::kAttribute: {
				return CreateRef<None>();
			}
			case Antomic::expr_::ExprKind::kBinOp: {
				auto expr = dynamic_cast<BinOp_t>(node);
				auto left = Eval(expr->left);
				auto right = Eval(expr->right);

				return CreateRef<None>();
			}
			case Antomic::expr_::ExprKind::kBoolOp: {
				return CreateRef<None>();
			}
			case Antomic::expr_::ExprKind::kCall: {
				return CreateRef<None>();
			}
			case Antomic::expr_::ExprKind::kCompare: {
				return CreateRef<None>();
			}
			case Antomic::expr_::ExprKind::kConstant: {
				return CreateRef<None>();
			}
			case Antomic::expr_::ExprKind::kDict: {
				return CreateRef<None>();
			}
			case Antomic::expr_::ExprKind::kFormattedValue: {
				return CreateRef<None>();
			}
			case Antomic::expr_::ExprKind::kIndex: {
				return CreateRef<None>();
			}
			case Antomic::expr_::ExprKind::kLambda: {
				return CreateRef<None>();
			}
			case Antomic::expr_::ExprKind::kList: {
				return CreateRef<None>();
			}
			case Antomic::expr_::ExprKind::kName: {
				return CreateRef<None>();
			}
			case Antomic::expr_::ExprKind::kSlice: {
				return CreateRef<None>();
			}
			case Antomic::expr_::ExprKind::kSubscript: {
				return CreateRef<None>();
			}
			case Antomic::expr_::ExprKind::kTuple: {
				return CreateRef<None>();
			}
			case Antomic::expr_::ExprKind::kUnaryOp: {
				return CreateRef<None>();
			}
		}
		return CreateRef<None>();
	}

	Ref<ScriptObject> ScriptEngine::Eval(expr_context_t node) { return nullptr; }

	Ref<ScriptObject> ScriptEngine::Eval(boolop_t node) { return nullptr; }

	Ref<ScriptObject> ScriptEngine::Eval(operator_t node) { return nullptr; }

	Ref<ScriptObject> ScriptEngine::Eval(unaryop_t node) { return nullptr; }

	Ref<ScriptObject> ScriptEngine::Eval(cmpop_t node) { return nullptr; }

	Ref<ScriptObject> ScriptEngine::Eval(excepthandler_t node) { return nullptr; }

	Ref<ScriptObject> ScriptEngine::Eval(arguments_t node) { return nullptr; }

	Ref<ScriptObject> ScriptEngine::Eval(arg_t node) { return nullptr; }

	Ref<ScriptObject> ScriptEngine::Eval(alias_t node) { return nullptr; }

	Ref<ScriptObject> ScriptEngine::Eval(const std::vector<Antomic::expr_t>& list)
	{
		return nullptr;
	}

	Ref<ScriptObject> ScriptEngine::Eval(const std::vector<Antomic::alias_t>& list)
	{
		return nullptr;
	}

	Ref<ScriptObject> ScriptEngine::Eval(const std::vector<Antomic::excepthandler_t>& list)
	{
		return nullptr;
	}

} // namespace Antomic