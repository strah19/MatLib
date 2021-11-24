#include "Parser.h"
#include "Logger.h"

namespace MatLib {
#define AST_NEW(type) \
    static_cast<type*>(DefaultAst(new type))

#define AST_DELETE(type) delete type

#define AST_CAST(type, base) static_cast<type*>(base)

	static std::map<int, int> precedences;

	Parser::Parser(Lexer* lexer) {
		precedences[Tok::T_PLUS] = 1;
		precedences[Tok::T_MINUS] = 1;
		precedences[Tok::T_STAR] = 2;
		precedences[Tok::T_SLASH] = 2;

		this->lexer = lexer;
	}

	Ast_Identifier* Parser::ParseId() {
		auto id = AST_NEW(Ast_Identifier);
		id->id = Next()->id;
		return id;
	}

	Ast_Expression* Parser::ParseUnary() {
		auto unary = AST_NEW(Ast_UnaryExpression);
		switch (Peek()->type) {
		case Tok::T_PLUS: {
			Match(Tok::T_PLUS);
			unary->op = AST_UNARY_PLUS;
			break;
		}
		case Tok::T_MINUS: {
			Match(Tok::T_MINUS);
			unary->op = AST_UNARY_MINUS;
			break;
		}
		case Tok::T_LPAR: {
			Match(Tok::T_LPAR);
			unary->op = AST_UNARY_NESTED;
			//Run parse_expression here to parse the expression in the ()
			break;
		}
		default: {
			AST_DELETE(unary);
			return ParsePrimary();
		}
		}

		unary->next = ParseUnary();
		return unary;
	}

	Ast_Expression* Parser::ParsePrimary() {
		auto prime = AST_NEW(Ast_PrimaryExpression);
		switch (Peek()->type) {
		case Tok::T_NUM_CONST: {
			prime->num_const = Peek()->num_const;
			Match(Tok::T_NUM_CONST);
			break;
		}
		case Tok::T_IDENTIFIER: {
			prime->ident = ParseId();
			break;
		}
		default: {
			AST_DELETE(prime);
			return nullptr;
		}
		}
		return prime;
	}

	Ast_BinaryExpression* Parser::ParseBinary() {
		auto expr = AST_NEW(Ast_BinaryExpression);
		switch (Peek()->type) {
		case Tok::T_STAR:
			expr->op = AST_OPERATOR_MULTIPLICATIVE;
			Match(Tok::T_STAR);
			break;
		case Tok::T_PLUS:
			expr->op = AST_OPERATOR_ADD;
			Match(Tok::T_PLUS);
			break;
		case Tok::T_MINUS:
			expr->op = AST_OPERATOR_SUB;
			Match(Tok::T_MINUS);
			break;
		case Tok::T_SLASH:
			expr->op = AST_OPERATOR_DIVISION;
			Match(Tok::T_SLASH);
			break;
		case Tok::T_PERCENT:
			expr->op = AST_OPERATOR_MODULO;
			Match(Tok::T_PERCENT);
			break;
		case Tok::T_LARROW:
			expr->op = AST_OPERATOR_LT;
			Match(Tok::T_LARROW);
			break;
		case Tok::T_RARROW:
			expr->op = AST_OPERATOR_GT;
			Match(Tok::T_RARROW);
			break;
		case Tok::T_DOUBLE_EQUAL:
			expr->op = AST_OPERATOR_COMPARITIVE_EQUAL;
			Match(Tok::T_DOUBLE_EQUAL);
			break;
		case Tok::T_LTE:
			expr->op = AST_OPERATOR_LTE;
			Match(Tok::T_LTE);
			break;
		case Tok::T_GTE:
			expr->op = AST_OPERATOR_GTE;
			Match(Tok::T_GTE);
			break;
		case Tok::T_NOT:
			expr->op = AST_OPERATOR_COMPARITIVE_NOT_EQUAL;
			Match(Tok::T_NOT);
			break;
		}

		return expr;
	}

	Ast_ProcedureCall* Parser::ParseProcedureCall() {
		return nullptr;

	}
	/*
	Ast_Expression* Parser::ParseExpression() {
		auto lexpr = ParseUnary();

		auto expr = ParseBinary();

		if (expr->op == AST_OPERATOR_NONE) {
			AST_DELETE(expr);
			return lexpr;
		}

		expr->left = lexpr;
		expr->right = ParseExpression();

		return expr;
	}
	*/


	Ast_Expression* Parser::ParseExpression(int prec) {
		Ast_BinaryExpression* bin = AST_NEW(Ast_BinaryExpression);
		while (Peek()->type != Tok::T_NEWLINE) {
			auto lexpr = ParseUnary();
			if (precedences[Peek()->type] >= prec) {
				auto b = ParseBinary(); 
				if (b) {
					bin->op = b->op;
					bin->right = ParseExpression(precedences[PeekOff(-1)->type] + 1);
					if (lexpr != nullptr)
						bin->left = lexpr;

					if (Peek()->type == Tok::T_NEWLINE) break;

					auto temp = AST_NEW(Ast_BinaryExpression);
					temp->left = bin;
					bin = temp;
				}
			}
			else {
				AST_DELETE(bin);
				return lexpr;
			}
		}	

		return bin;
	}

	Ast_Statement* Parser::ParseStatement() {
		if (Peek()->type == Tok::T_IDENTIFIER) {
			if (PeekOff(1)->type == Tok::T_EQUAL) {
				//Assignment
				auto assignment = AST_NEW(Ast_Assignment);
				assignment->id = ParseId();
				Match(Tok::T_EQUAL);
				assignment->expr = ParseExpression();
				return assignment;
			}
			else if (PeekOff(1)->type == Tok::T_LPAR) {
				//Procedure

			}
			else 
				return nullptr;
		}
		else {
			EMBER_LOG_ERROR("Expected an identifier for statement.");
			Next();
		}

		return nullptr;
	}

	void Parser::Run() {
		index = 0;
		root = AST_NEW(Ast_Script);

		while (Peek()->type != Tok::T_EOF) {
			auto proc = ParseStatement();
			if (proc)
				root->procedures.push_back(proc);
		}
	}

	Token* Parser::Peek() {
		return (index < lexer->Tokens().size()) ? &lexer->Tokens()[index] : nullptr;
	}

	Token* Parser::PeekOff(int off) {
		return (index + off < lexer->Tokens().size()) ? &lexer->Tokens()[index + off] : nullptr;
	}

	Token* Parser::Next() {
		return (index < lexer->Tokens().size()) ? &lexer->Tokens()[index++] : nullptr;
	}

	void Parser::Match(int type) {
		if (Peek()->type != type)
			EMBER_LOG_ERROR("Expected '%d' on line %d.", type, Peek()->line);
		Next();
	}

	Ast* Parser::DefaultAst(Ast* ast) {
		ast->line = Peek()->line;
		return ast;
	}

	Parser::~Parser() {
		Destroy();
	}

	void Parser::Destroy() {
		delete root;
		root = nullptr;
	}

	void Parser::Visualize() {
		if (root) {
			for (auto& proc : root->procedures) {
				switch (proc->type) {
				case AST_ASSIGNMENT:
					auto assign = AST_CAST(Ast_Assignment, proc);
					printf("Assignment: %s\n", assign->id->id.c_str());
					if (assign->expr) {
						VisualizeExpression(assign->expr);
					}
					break;
				}
			}
		}
	}

	void Parser::VisualizeExpression(Ast_Expression* expr, int indent) {
		if (expr) {
			switch (expr->type) {
			case AST_UNARY: {
				auto u = AST_CAST(Ast_UnaryExpression, expr);
				Ident(indent);
				printf("Unary: %d\n", u->op);
				if (u->next) {
					VisualizeExpression(u->next, indent + 1);
				}
				break;
			}
			case AST_PRIMARY: {
				auto p = AST_CAST(Ast_PrimaryExpression, expr);
				Ident(indent);
				printf("Primary: %f\n", p->num_const);
				break;
			}
			case AST_BINARY: {
				auto b = AST_CAST(Ast_BinaryExpression, expr);
				Ident(indent);
				printf("Binary: %d\n", b->op);
				VisualizeExpression(b->left, indent + 1);
				VisualizeExpression(b->right, indent + 1);
				break;
			}
			}
		}
	}

	void Parser::Ident(int indent) {
		while (indent > 0) {
			printf("\t");
			indent--;
		}
	}
}