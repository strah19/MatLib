#include "Parser.h"
#include "Logger.h"

namespace MatLib {
	Parser::Parser(Lexer* lexer) {
		this->lexer = lexer;
	}

	int Parser::TokenTypeToAstType(Token* token) {
		switch (token->type) {
		case Tok::T_PLUS:
			return AST_OPERATOR_ADD;
		case Tok::T_MINUS:
			return AST_OPERATOR_SUB;
		case Tok::T_STAR:
			return AST_OPERATOR_MULTIPLICATIVE;
		case Tok::T_SLASH:
			return AST_OPERATOR_DIVISION;
		}
		return AST_OPERATOR_NONE;
	}

	Ast_Identifier* Parser::ParseId() {
		auto id = AST_NEW(Ast_Identifier);
		if (Peek()->type = Tok::T_IDENTIFIER)
			id->id = Advance()->id;
		else
			id = nullptr;
		return id;
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
		case Tok::T_LPAR: {
			Match(Tok::T_LPAR);
			auto expr = ParseExpression();
			Match(Tok::T_RPAR);
			prime->nested = expr;
			break;
		}
		default: {
			AST_DELETE(prime);
			return nullptr;
		}
		}
		return prime;
	}

	Ast_Expression* Parser::ParseUnary() {
		if (Match(Tok::T_MINUS)) {
			Ast_Expression* right = ParseUnary();
			return new Ast_UnaryExpression(right, AST_UNARY_MINUS);
		}

		return ParsePrimary();
	}

	Ast_Expression* Parser::ParseFactor() {
		auto expr = ParseUnary();

		while (Match(Tok::T_SLASH) || Match(Tok::T_STAR)) {
			Token* op = Previous();
			auto right = ParseUnary();
			expr = new Ast_BinaryExpression(expr, TokenTypeToAstType(op), right);
		}

		return expr;
	}

	Ast_Expression* Parser::ParseExpression() {
		auto expr = ParseFactor();

		while (Match(Tok::T_MINUS) || Match(Tok::T_PLUS)) {
			auto t = Previous();
			auto right = ParseFactor();
			expr = new Ast_BinaryExpression(expr, TokenTypeToAstType(t), right);
		}

		return expr;
	}

	Ast_ProcedureCall* Parser::ParseProcedureCall() {
		return nullptr;

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
			Advance();
		}

		return nullptr;
	}

	void Parser::Run() {
		token_index = 0;
		root = AST_NEW(Ast_Script);

		while (Peek()->type != Tok::T_EOF) {
			auto proc = ParseStatement();
			if (proc)
				root->procedures.push_back(proc);
		}
	}

	Token* Parser::Peek() {
		return (!AtEnd()) ? &lexer->Tokens()[token_index] : nullptr;
	}

	Token* Parser::PeekOff(int off) {
		return (token_index + off < lexer->Tokens().size()) ? &lexer->Tokens()[token_index + off] : nullptr;
	}

	Token* Parser::Advance() {
		return (!AtEnd()) ? &lexer->Tokens()[token_index++] : nullptr;
	}

	bool Parser::Match(int type) {
		if (!Check(type)) {
			return false;
		}
		Advance();
		return true;
	}

	bool Parser::Check(int type) {
		if (AtEnd())  return false;
		return (Peek()->type == type);
	}

	Token* Parser::Previous() {
		return &lexer->Tokens()[token_index - 1];
	}

	bool Parser::AtEnd() {
		return !(token_index < lexer->Tokens().size());
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
				if (p->nested) {
					printf("Nested: \n");
					VisualizeExpression(p->nested, indent + 1);
				}
				else
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