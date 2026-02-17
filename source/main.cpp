#include "ast.hpp"
#include "interpreter.hpp"
#include "node.hpp"
#include "parser.tab.hpp" // ← для yy::parser
#include <iostream>
#include <string>

// ===== РЕКУРСИВНЫЙ ДАМП ДЕРЕВА =====
static void dump_node(const language::INode* node, int indent = 0)
{
    if (!node)
        return;
    std::string prefix(indent, ' ');

    if (auto* n = dynamic_cast<const language::Number*>(node))
        std::cout << prefix << "Number(" << n->get_value() << ")\n";
    else if (auto* v = dynamic_cast<const language::Variable*>(node))
        std::cout << prefix << "Variable('" << v->get_name() << "')\n";
    else if (auto* op = dynamic_cast<const language::BinaryOp*>(node))
    {
        const char* sym = "?";
        switch (op->get_op())
        {
        case language::BinaryOp::Op::ADD:
            sym = "+";
            break;
        case language::BinaryOp::Op::SUB:
            sym = "-";
            break;
        case language::BinaryOp::Op::MUL:
            sym = "*";
            break;
        case language::BinaryOp::Op::DIV:
            sym = "/";
            break;
        case language::BinaryOp::Op::EQ:
            sym = "==";
            break;
        case language::BinaryOp::Op::NE:
            sym = "!=";
            break;
        case language::BinaryOp::Op::L:
            sym = "<";
            break;
        case language::BinaryOp::Op::G:
            sym = ">";
            break;
        case language::BinaryOp::Op::LE:
            sym = "<=";
            break;
        case language::BinaryOp::Op::GE:
            sym = ">=";
            break;
        case language::BinaryOp::Op::AND:
            sym = "&&";
            break;
        case language::BinaryOp::Op::OR:
            sym = "||";
            break;
        }
        std::cout << prefix << "BinaryOp(" << sym << ")\n";
        dump_node(op->get_left(), indent + 2);
        dump_node(op->get_right(), indent + 2);
    }
    else if (auto* u = dynamic_cast<const language::UnaryOp*>(node))
    {
        const char* sym =
            (u->get_op() == language::UnaryOp::Op::MINUS) ? "-" : "!";
        std::cout << prefix << "UnaryOp(" << sym << ")\n";
        dump_node(u->get_expr(), indent + 2);
    }
    else if (dynamic_cast<const language::ScanfExpr*>(node))
        std::cout << prefix << "ScanfExpr(?)\n";

    else if (auto* a = dynamic_cast<const language::Assignment*>(node))
    {
        std::cout << prefix << "Assignment(var='" << a->get_var_name()
                  << "')\n";
        std::cout << prefix << "  expr:\n";
        dump_node(a->get_expr(), indent + 4);
    }
    else if (auto* d = dynamic_cast<const language::Declaration*>(node))
    {
        std::cout << prefix << "Declaration(var='" << d->get_var_name() << "')";
        if (d->get_expr())
        {
            std::cout << "\n" << prefix << "  expr:\n";
            dump_node(d->get_expr(), indent + 4);
        }
        else
        {
            std::cout << " (uninitialized)\n";
        }
    }
    else if (auto* p = dynamic_cast<const language::PrintStmt*>(node))
    {
        std::cout << prefix << "PrintStmt\n";
        std::cout << prefix << "  expr:\n";
        dump_node(p->get_expr(), indent + 4);
    }
    else if (auto* i = dynamic_cast<const language::IfStmt*>(node))
    {
        std::cout << prefix << "IfStmt\n";
        std::cout << prefix << "  condition:\n";
        dump_node(i->get_condition(), indent + 4);
        std::cout << prefix << "  then:\n";
        dump_node(i->get_body_if(), indent + 4);
        if (i->get_body_else())
        {
            std::cout << prefix << "  else:\n";
            dump_node(i->get_body_else(), indent + 4);
        }
    }
    else if (auto* w = dynamic_cast<const language::WhileStmt*>(node))
    {
        std::cout << prefix << "WhileStmt\n";
        std::cout << prefix << "  condition:\n";
        dump_node(w->get_condition(), indent + 4);
        std::cout << prefix << "  body:\n";
        dump_node(w->get_body(), indent + 4);
    }
    else if (auto* b = dynamic_cast<const language::BlockStmt*>(node))
    {
        std::cout << prefix << "BlockStmt [" << b->get_statement_count()
                  << " stmts]\n";
        for (size_t i = 0; i < b->get_statement_count(); ++i)
            dump_node(b->get_statement(i), indent + 2);
    }
}
#include "ast.hpp"
#include "interpreter.hpp"
#include "node.hpp"
#include "parser.tab.hpp"
#include <iostream>
#include <string>

int main()
{
    language::AST ast;

    yy::parser parser(&ast);

    std::cerr << "Enter code (Ctrl+D when done):\n";
    int result = parser.parse();

    if (result == 0)
    {

        language::Interpreter interp;
        interp.scope_stack.AddScope();

        try
        {

            ast.get_root()->evaluate(interp);
        }
        catch (const std::exception& e)
        {
        }
    }

    return 0;
}
