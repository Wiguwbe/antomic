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
#include "gtest/gtest.h"
#include "Script/Parser/Parser.h"
#include "Script/Parser/Ast.h"

#define TEST_EXPRESSION(x, y)                 \
    {                                         \
        Antomic::Parser parser;               \
        auto expr = parser.FromExpression(x); \
        EXPECT_EQ(y, ToString(expr));         \
    }

static uint32_t currentIdentation = 0;

#define INDENT() currentIdentation += 4
#define DEDENT() currentIdentation -= 4
#define IDENTATION() std::string(currentIdentation, ' ')
#define NEWLINE() std::string(1, '\n')
#define ListWithComma(x, y) \
    bool first = true;      \
    for (auto e : x)        \
    {                       \
        if (!first)         \
        {                   \
            y += ",";       \
        }                   \
        y += ToString(e);   \
        first = false;      \
    }
#define List(x, y)        \
    for (auto e : x)      \
    {                     \
        y += ToString(e); \
    }

std::string ToString(Antomic::mod_t node);
std::string ToString(Antomic::stmt_t node);
std::string ToString(Antomic::expr_t node);
std::string ToString(Antomic::expr_context_t node);
std::string ToString(Antomic::boolop_t node);
std::string ToString(Antomic::operator_t node);
std::string ToString(Antomic::unaryop_t node);
std::string ToString(Antomic::cmpop_t node);
std::string ToString(Antomic::excepthandler_t node);
std::string ToString(Antomic::arguments_t node);
std::string ToString(Antomic::arg_t node);
std::string ToString(Antomic::alias_t node);
std::string ToString(const std::vector<Antomic::expr_t> &list);
std::string ToString(const std::vector<Antomic::alias_t> &list);
std::string ToString(const std::vector<Antomic::excepthandler_t> &list);

std::string ToString(Antomic::mod_t node)
{
    switch (node->mod_kind)
    {
    case Antomic::mod_::ModKind::kModule:
    {
        auto result = std::string("");
        auto mod = dynamic_cast<Antomic::Module_t>(node);
        List(mod->body, result);
        return result;
    }
    case Antomic::mod_::ModKind::kExpression:
    {
        auto result = std::string("");
        auto expr = dynamic_cast<Antomic::Expression_t>(node);
        return ToString(expr->body);
    }
    }
    return "";
}

std::string ToString(Antomic::stmt_t node)
{
    switch (node->stmt_kind)
    {
    case Antomic::stmt_::StmtKind::kAssert:
    {
        auto stmt = dynamic_cast<Antomic::Assert_t>(node);
        return IDENTATION() + "Assert(" + ToString(stmt->test) + (stmt->msg ? "," + ToString(stmt->msg) : "") + ")" + NEWLINE();
    }
    case Antomic::stmt_::StmtKind::kAssign:
    {
        auto stmt = dynamic_cast<Antomic::Assign_t>(node);
        return IDENTATION() + "Assign(" + ToString(stmt->target) + "," + ToString(stmt->value) + ")" + NEWLINE();
    }
    case Antomic::stmt_::StmtKind::kAugAssign:
    {
        auto stmt = dynamic_cast<Antomic::AugAssign_t>(node);
        return IDENTATION() + "AssignAug(" + ToString(stmt->target) + "," + ToString(stmt->op) + "," + ToString(stmt->value) + ")" + NEWLINE();
    }
    case Antomic::stmt_::StmtKind::kBreak:
        return IDENTATION() + "Break" + NEWLINE();
    case Antomic::stmt_::StmtKind::kClassDef:
    {
        auto stmt = dynamic_cast<Antomic::ClassDef_t>(node);
        auto result = IDENTATION() + "Class(" + stmt->name + "," + ToString(stmt->bases) + "):" + NEWLINE();
        INDENT();
        List(stmt->body, result);
        DEDENT();
        return result;
    }
    case Antomic::stmt_::StmtKind::kContinue:
        return IDENTATION() + "Continue" + NEWLINE();
    case Antomic::stmt_::StmtKind::kDelete:
    {
        auto stmt = dynamic_cast<Antomic::Delete_t>(node);
        return IDENTATION() + "Delete(" + ToString(stmt->target) + ")" + NEWLINE();
    }
    case Antomic::stmt_::StmtKind::kExpr:
    {
        auto stmt = dynamic_cast<Antomic::Expr_t>(node);
        return IDENTATION() + "Expr(" + ToString(stmt->value) + ")" + NEWLINE();
    }
    case Antomic::stmt_::StmtKind::kFor:
    {
        auto stmt = dynamic_cast<Antomic::For_t>(node);
        auto result = IDENTATION() + "For(" + ToString(stmt->target) + "," + ToString(stmt->iter) + "):" + NEWLINE();
        INDENT();
        List(stmt->body, result);
        DEDENT();
        return result;
    }
    case Antomic::stmt_::StmtKind::kFunctionDef:
    {
        auto stmt = dynamic_cast<Antomic::FunctionDef_t>(node);
        auto result = IDENTATION() + "FunctionDef(" + stmt->name + "," + ToString(stmt->args) + "):" + NEWLINE();
        INDENT();
        List(stmt->body, result);
        DEDENT();
        return result;
    }
    case Antomic::stmt_::StmtKind::kIf:
    {
        auto stmt = dynamic_cast<Antomic::If_t>(node);
        auto result = IDENTATION() + "If(" + ToString(stmt->test) + "):" + NEWLINE();
        INDENT();
        List(stmt->body, result);
        DEDENT();
        if (stmt->orelse.size() > 0)
        {
            result += IDENTATION() + "OrElse():" + NEWLINE();
            INDENT();
            List(stmt->orelse, result);
            DEDENT();
        }
        return result;
    }

    case Antomic::stmt_::StmtKind::kImport:
    {
        auto stmt = dynamic_cast<Antomic::Import_t>(node);
        return IDENTATION() + "Import(" + ToString(stmt->names) + ")" + NEWLINE();
    }
    case Antomic::stmt_::StmtKind::kImportFrom:
    {
        auto stmt = dynamic_cast<Antomic::ImportFrom_t>(node);
        return IDENTATION() + "ImportFrom(" + stmt->module + "," + ToString(stmt->names) + ")" + NEWLINE();
    }
    case Antomic::stmt_::StmtKind::kPass:
    {
        return IDENTATION() + "Pass()" + NEWLINE();
    }
    case Antomic::stmt_::StmtKind::kRaise:
    {
        auto stmt = dynamic_cast<Antomic::Raise_t>(node);
        return IDENTATION() + "Raise(" + ToString(stmt->exc) + ")" + NEWLINE();
    }
    case Antomic::stmt_::StmtKind::kReturn:
    {
        auto stmt = dynamic_cast<Antomic::Return_t>(node);
        return IDENTATION() + "Return(" + (stmt->value ? ToString(stmt->value) : "") + ")" + NEWLINE();
    }
    case Antomic::stmt_::StmtKind::kTry:
    {
        auto stmt = dynamic_cast<Antomic::Try_t>(node);
        auto result = IDENTATION() + "Try():" + NEWLINE();
        INDENT();
        List(stmt->body, result);
        DEDENT();
        if (stmt->handlers.size() > 0)
        {
            List(stmt->handlers, result);
        }
        if (stmt->finalbody.size() > 0)
        {
            result += IDENTATION() + "Finally():" + NEWLINE();
            INDENT();
            List(stmt->finalbody, result);
            DEDENT();
        }
        return result;
    }
    case Antomic::stmt_::StmtKind::kWhile:
    {
        return "";
    }
    }
    return "";
}

std::string ToString(Antomic::expr_t node)
{
    if (!node)
    {
        return "";
    }

    switch (node->expr_kind)
    {
    case Antomic::expr_::ExprKind::kAttribute:
    {
        auto expr = dynamic_cast<Antomic::Attribute_t>(node);
        return "Attribute(" + expr->name + "," + ToString(expr->value) + ")";
    }
    case Antomic::expr_::ExprKind::kBinOp:
    {
        auto expr = dynamic_cast<Antomic::BinOp_t>(node);
        return "BinOp(" + ToString(expr->left) + "," + ToString(expr->op) + "," + ToString(expr->right) + ")";
    }
    case Antomic::expr_::ExprKind::kBoolOp:
    {
        auto expr = dynamic_cast<Antomic::BoolOp_t>(node);
        return "BoolOp(" + ToString(expr->left) + "," + ToString(expr->op) + "," + ToString(expr->right) + ")";
    }
    case Antomic::expr_::ExprKind::kCall:
    {
        auto expr = dynamic_cast<Antomic::Call_t>(node);
        return "Call(" + ToString(expr->func) + ",Args(" + ToString(expr->args) + "))";
    }
    case Antomic::expr_::ExprKind::kCompare:
    {
        auto expr = dynamic_cast<Antomic::Compare_t>(node);
        return "Compare(" + ToString(expr->left) + "," + ToString(expr->op) + "," + ToString(expr->right) + ")";
    }
    case Antomic::expr_::ExprKind::kConstant:
    {
        auto expr = dynamic_cast<Antomic::Constant_t>(node);
        switch (expr->kind)
        {
        case Antomic::kind_t::kFloat:
            return std::to_string(std::any_cast<float>(expr->value));
        case Antomic::kind_t::kInt:
            return std::to_string(std::any_cast<int>(expr->value));
        case Antomic::kind_t::kString:
            return "\"" + std::any_cast<std::string>(expr->value) + "\"";
        case Antomic::kind_t::kObject:
            return "<object>";
        default:
            break;
        }
        return "";
    }
    case Antomic::expr_::ExprKind::kDict:
    {
        auto expr = dynamic_cast<Antomic::Dict_t>(node);
        std::string result = "Dict(";
        for (int i = 0; i < expr->keys.size(); i++)
        {
            if (i)
            {
                result += ",";
            }
            result += "KeyValue(" + ToString(expr->keys[i]) + "," + ToString(expr->values[i]) + ")";
        }
        result += ")";
        return result;
    }
    case Antomic::expr_::ExprKind::kFormattedValue:
    {
        return "";
    }
    case Antomic::expr_::ExprKind::kIndex:
    {
        auto expr = dynamic_cast<Antomic::Index_t>(node);
        return "Index(" + ToString(expr->value) + ")";
    }
    case Antomic::expr_::ExprKind::kLambda:
    {
        auto expr = dynamic_cast<Antomic::Lambda_t>(node);
        return "Lambda(" + ToString(expr->args) + "," + ToString(expr->body) + ")";
    }
    case Antomic::expr_::ExprKind::kList:
    {
        auto expr = dynamic_cast<Antomic::List_t>(node);
        return "List(" + ToString(expr->eltsRead) + ")";
    }
    case Antomic::expr_::ExprKind::kName:
    {
        auto expr = dynamic_cast<Antomic::Name_t>(node);
        return expr->idRead;
    }
    case Antomic::expr_::ExprKind::kSlice:
    {
        auto expr = dynamic_cast<Antomic::Slice_t>(node);
        return "Slice(Lower(" + (expr->lower ? ToString(expr->lower) : "") + "),Upper(" + (expr->upper ? ToString(expr->upper) : "") + "),Step(" + (expr->step ? ToString(expr->step) : "") + "))";
    }
    case Antomic::expr_::ExprKind::kSubscript:
    {
        auto expr = dynamic_cast<Antomic::Subscript_t>(node);
        return "Subscript(" + ToString(expr->value) + "," + ToString(expr->sliceRead) + ")";
    }
    case Antomic::expr_::ExprKind::kTuple:
    {
        auto expr = dynamic_cast<Antomic::Tuple_t>(node);
        return "Tuple(" + ToString(expr->eltsRead) + ")";
    }
    case Antomic::expr_::ExprKind::kUnaryOp:
    {
        auto expr = dynamic_cast<Antomic::UnaryOp_t>(node);
        return "UnaryOp(" + ToString(expr->op) + "," + ToString(expr->operand) + ")";
    }
    }
    return "";
}

std::string ToString(Antomic::expr_context_t node)
{
    return "";
}

std::string ToString(Antomic::boolop_t op)
{
    switch (op)
    {
    case Antomic::boolop_t::kAnd:
        return "and";
    case Antomic::boolop_t::kOr:
        return "or";
    }
    return "";
}

std::string ToString(Antomic::operator_t op)
{
    switch (op)
    {
    case Antomic::operator_t::kAdd:
        return "+";
    case Antomic::operator_t::kSub:
        return "-";
    case Antomic::operator_t::kMult:
        return "*";
    case Antomic::operator_t::kMatMult:
        return "*";
    case Antomic::operator_t::kDiv:
        return "/";
    case Antomic::operator_t::kMod:
        return "%";
    case Antomic::operator_t::kPow:
        return "**";
    case Antomic::operator_t::kLShift:
        return "<<";
    case Antomic::operator_t::kRShift:
        return ">>";
    case Antomic::operator_t::kBitOr:
        return "|";
    case Antomic::operator_t::kBitXor:
        return "^";
    case Antomic::operator_t::kBitAnd:
        return "&";
    case Antomic::operator_t::kFloorDiv:
        return "//";
    }
    return "";
}

std::string ToString(Antomic::unaryop_t op)
{
    switch (op)
    {
    case Antomic::unaryop_t::kInvert:
        return "~";
    case Antomic::unaryop_t::kNot:
        return "not";
    case Antomic::unaryop_t::kUAdd:
        return "+";
    case Antomic::unaryop_t::kUSub:
        return "-";
    }
    return "";
}

std::string ToString(Antomic::cmpop_t op)
{
    switch (op)
    {
    case Antomic::cmpop_t::kEq:
        return "==";
    case Antomic::cmpop_t::kGt:
        return ">";
    case Antomic::cmpop_t::kGtE:
        return ">=";
    case Antomic::cmpop_t::kIn:
        return "in";
    case Antomic::cmpop_t::kIs:
        return "is";
    case Antomic::cmpop_t::kIsNot:
        return "is not";
    case Antomic::cmpop_t::kLt:
        return "<";
    case Antomic::cmpop_t::kLtE:
        return "<=";
    case Antomic::cmpop_t::kNotEq:
        return "!=";
    case Antomic::cmpop_t::kNotIn:
        return "not in";
    }
    return "";
}

std::string ToString(Antomic::excepthandler_t node)
{
    auto ex = dynamic_cast<Antomic::ExceptHandler_t>(node);
    auto result = IDENTATION() + std::string("Except(") + ToString(ex->type) + "," + ex->name + "):" + NEWLINE();
    INDENT();
    List(ex->body, result);
    DEDENT();
    return result;
}

std::string ToString(Antomic::arguments_t node)
{
    auto result = std::string("Args(");
    bool first = true;
    for (auto arg : node->args)
    {
        if (!first)
        {
            result += ",";
        }
        result += ToString(arg);
        first = false;
    }
    return result + ")";
}

std::string ToString(Antomic::arg_t node)
{
    auto result = std::string("Arg(") + node->arg;
    switch (node->kind)
    {
    case Antomic::kind_t::kFloat:
        result += ",Float";
        break;
    case Antomic::kind_t::kInt:
        result += ",Int";
        break;
    case Antomic::kind_t::kObject:
        result += ",Object";
        break;
    case Antomic::kind_t::kString:
        result += ":String";
        break;
    default:
        break;
    }
    return result + ")";
}

std::string ToString(Antomic::alias_t node)
{
    return "Alias(" + node->name + (node->asname == "" ? "" : ("," + node->asname)) + ")";
}

std::string ToString(const std::vector<Antomic::expr_t> &list)
{
    std::string result = "";
    ListWithComma(list, result);
    return result;
}

std::string ToString(const std::vector<Antomic::alias_t> &list)
{
    std::string result = "";
    ListWithComma(list, result);
    return result;
}

std::string ToString(const std::vector<Antomic::excepthandler_t> &list)
{
    std::string result = "";
    List(list, result);
    return result;
}

TEST(AntomicCoreTest, ParserTests)
{
    TEST_EXPRESSION("a+b", "BinOp(a,+,b)");
    TEST_EXPRESSION("(a+b)+c", "BinOp(BinOp(a,+,b),+,c)");
    TEST_EXPRESSION("a+(b+c)", "BinOp(a,+,BinOp(b,+,c))");
    TEST_EXPRESSION("(a+b)+c", "BinOp(BinOp(a,+,b),+,c)");

    TEST_EXPRESSION("a*b", "BinOp(a,*,b)");
    TEST_EXPRESSION("(a*b)*c", "BinOp(BinOp(a,*,b),*,c)");
    TEST_EXPRESSION("a*(b*c)", "BinOp(a,*,BinOp(b,*,c))");
    TEST_EXPRESSION("(a*b)*c", "BinOp(BinOp(a,*,b),*,c)");

    TEST_EXPRESSION("(a+b)*c", "BinOp(BinOp(a,+,b),*,c)");
    TEST_EXPRESSION("a*(b+c)", "BinOp(a,*,BinOp(b,+,c))");

    TEST_EXPRESSION("(a*b)+c", "BinOp(BinOp(a,*,b),+,c)");
    TEST_EXPRESSION("a+(b*c)", "BinOp(a,+,BinOp(b,*,c))");

    TEST_EXPRESSION("range(1)", "Call(range,Args(1))");
    TEST_EXPRESSION("a.range(1)", "Call(Attribute(range,a),Args(1))");

    TEST_EXPRESSION("color(0xf)", "Call(color,Args(15))");

    Antomic::Parser parser;
    auto mod1 = parser.FromFile("tests/files/parser_test.py");

    if (mod1)
    {
        std::cout << ToString(mod1);
    }
}