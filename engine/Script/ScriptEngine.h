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
#include "Script/Parser/Parser.h"
#include "ScriptObject.h"
#include "ScriptScope.h"

namespace Antomic
{
	class ScriptEngine
	{
	public:
		ScriptEngine();
		~ScriptEngine();

	public:
		Ref<ScriptObject> Eval(const std::string& expr);

	private:
		Ref<ScriptObject> Eval(mod_t node);
		Ref<ScriptObject> Eval(stmt_t node);
		Ref<ScriptObject> Eval(expr_t node);
		Ref<ScriptObject> Eval(expr_context_t node);
		Ref<ScriptObject> Eval(boolop_t node);
		Ref<ScriptObject> Eval(operator_t node);
		Ref<ScriptObject> Eval(unaryop_t node);
		Ref<ScriptObject> Eval(cmpop_t node);
		Ref<ScriptObject> Eval(excepthandler_t node);
		Ref<ScriptObject> Eval(arguments_t node);
		Ref<ScriptObject> Eval(arg_t node);
		Ref<ScriptObject> Eval(alias_t node);
		Ref<ScriptObject> Eval(const std::vector<expr_t>& list);
		Ref<ScriptObject> Eval(const std::vector<alias_t>& list);
		Ref<ScriptObject> Eval(const std::vector<excepthandler_t>& list);

		// Boolean Operators
		Ref<ScriptObject> And(Ref<ScriptObject>& left, Ref<ScriptObject>& right);
		Ref<ScriptObject> Or(Ref<ScriptObject>& left, Ref<ScriptObject>& right);

		// Binary Operators
		Ref<ScriptObject> Add(Ref<ScriptObject>& left, Ref<ScriptObject>& right);
		Ref<ScriptObject> Sub(Ref<ScriptObject>& left, Ref<ScriptObject>& right);
		Ref<ScriptObject> Mult(Ref<ScriptObject>& left, Ref<ScriptObject>& right);
		Ref<ScriptObject> MatMult(Ref<ScriptObject>& left, Ref<ScriptObject>& right);
		Ref<ScriptObject> Div(Ref<ScriptObject>& left, Ref<ScriptObject>& right);
		Ref<ScriptObject> Mod(Ref<ScriptObject>& left, Ref<ScriptObject>& right);
		Ref<ScriptObject> Pow(Ref<ScriptObject>& left, Ref<ScriptObject>& right);
		Ref<ScriptObject> LShift(Ref<ScriptObject>& left, Ref<ScriptObject>& right);
		Ref<ScriptObject> RShift(Ref<ScriptObject>& left, Ref<ScriptObject>& right);
		Ref<ScriptObject> BitOr(Ref<ScriptObject>& left, Ref<ScriptObject>& right);
		Ref<ScriptObject> BitXor(Ref<ScriptObject>& left, Ref<ScriptObject>& right);
		Ref<ScriptObject> BitAnd(Ref<ScriptObject>& left, Ref<ScriptObject>& right);
		Ref<ScriptObject> FloorDiv(Ref<ScriptObject>& left, Ref<ScriptObject>& right);

		// Unary Operators
		Ref<ScriptObject> Invert(Ref<ScriptObject>& value);
		Ref<ScriptObject> Not(Ref<ScriptObject>& value);
		Ref<ScriptObject> UAdd(Ref<ScriptObject>& value);
		Ref<ScriptObject> USub(Ref<ScriptObject>& value);

		// Compare Operators
		Ref<ScriptObject> Eq(Ref<ScriptObject>& left, Ref<ScriptObject>& right);
		Ref<ScriptObject> Gt(Ref<ScriptObject>& left, Ref<ScriptObject>& right);
		Ref<ScriptObject> GtE(Ref<ScriptObject>& left, Ref<ScriptObject>& right);
		Ref<ScriptObject> In(Ref<ScriptObject>& left, Ref<ScriptObject>& right);
		Ref<ScriptObject> Is(Ref<ScriptObject>& left, Ref<ScriptObject>& right);
		Ref<ScriptObject> IsNot(Ref<ScriptObject>& left, Ref<ScriptObject>& right);
		Ref<ScriptObject> Lt(Ref<ScriptObject>& left, Ref<ScriptObject>& right);
		Ref<ScriptObject> LtE(Ref<ScriptObject>& left, Ref<ScriptObject>& right);
		Ref<ScriptObject> NotEq(Ref<ScriptObject>& left, Ref<ScriptObject>& right);
		Ref<ScriptObject> NotIn(Ref<ScriptObject>& left, Ref<ScriptObject>& right);

	private:
		Ref<ScriptScope> mScope;
	};
} // namespace Antomic