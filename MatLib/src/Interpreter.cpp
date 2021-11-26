#include "Interpreter.h"

namespace MatLib {
	Interpreter::Interpreter(Parser* parser) {
		this->parser = parser;
	}

	double Interpreter::SolveExpression(Ast_Expression* expr) {
		if (expr) {
			switch (expr->type) {
			case AST_UNARY: {
				auto u = AST_CAST(Ast_UnaryExpression, expr);
				double p = SolveExpression(u->next);
				switch (u->op) {
				case AST_UNARY_MINUS:
					return -p;
				default:
					return p;
				}
				break;
			}
			case AST_PRIMARY: {
				auto p = AST_CAST(Ast_PrimaryExpression, expr);
				
				if (p->nested) {
					return SolveExpression(p->nested);
				}
				else 
					return p->num_const;
				break;
			}
			case AST_BINARY: {
				auto b = AST_CAST(Ast_BinaryExpression, expr);
				double left = SolveExpression(b->left);
				double right = SolveExpression(b->right);

				switch (b->op) {
				case AST_OPERATOR_ADD:
					return left + right;
				case AST_OPERATOR_SUB:
					return left - right;
				case AST_OPERATOR_MULTIPLICATIVE:
					return left * right;
				case AST_OPERATOR_DIVISION:
					return left / right;
				default:
					break;
				}

				break;
			}
			}
		}
		return 0.0;
	}
}