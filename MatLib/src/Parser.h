#ifndef PARSER_H
#define PARSER_H

#include "Lexer.h"

namespace MatLib {
	struct Ast_Expression;

	enum {
		AST_ID,
		AST_EXPRESSION,
		AST_UNARY,
		AST_PRIMARY,
		AST_BINARY,
		AST_ASSIGNMENT,
		AST_PROCEDURE,
		AST_PROCEDURE_CALL,
		AST_STATEMENT,
		AST_SCRIPT
	};

	enum {
		AST_OPERATOR_MULTIPLICATIVE,
		AST_OPERATOR_DIVISION,
		AST_OPERATOR_MODULO,
		AST_OPERATOR_ADD,
		AST_OPERATOR_SUB,
		AST_OPERATOR_COMPARITIVE_EQUAL,
		AST_OPERATOR_COMPARITIVE_NOT_EQUAL,
		AST_OPERATOR_LTE,
		AST_OPERATOR_GTE,
		AST_OPERATOR_LT,
		AST_OPERATOR_GT,
		AST_OPERATOR_AND,
		AST_OPERATOR_OR,
		AST_OPERATOR_POWER,
		AST_OPERATOR_NONE
	};

	enum {
		AST_UNARY_PLUS,
		AST_UNARY_MINUS,
		AST_UNARY_NESTED,
		AST_UNARY_NONE
	};

	struct Ast {
		uint32_t line = 0;
		int type = 0;
	};

	struct Ast_Identifier : public Ast {
		Ast_Identifier() { type = AST_ID; }
		std::string id = "";
	};

	struct Ast_ProcedureCall : public Ast {
		Ast_ProcedureCall() { type = AST_PROCEDURE_CALL; }
		~Ast_ProcedureCall() { delete id; }

		Ast_Identifier* id = nullptr;
		std::vector<Ast_Expression> args;
	};

	struct Ast_Expression : public Ast {
		Ast_Expression() { type = AST_EXPRESSION; }
		~Ast_Expression() { delete next; }

		Ast_Expression* next = nullptr;

		Ast_Expression(const Ast_Expression& expr) {
			next = expr.next;
		}
	};

	struct Ast_PrimaryExpression : public Ast_Expression {
		Ast_PrimaryExpression() { type = AST_PRIMARY; }
		~Ast_PrimaryExpression() { delete ident; delete call; }

		double num_const = 0.0;
		Ast_Identifier* ident = nullptr;
		Ast_ProcedureCall* call = nullptr;
	};

	struct Ast_BinaryExpression : public Ast_Expression {
		Ast_BinaryExpression() { type = AST_BINARY; }
		~Ast_BinaryExpression() { delete left; delete right; }

		int op = AST_OPERATOR_NONE;

		Ast_Expression* left = nullptr;
		Ast_Expression* right = nullptr;

		Ast_BinaryExpression(const Ast_BinaryExpression& bin) {
			op = bin.op;
			left = bin.left;
			right = bin.right;
		}
	};

	struct Ast_UnaryExpression : public Ast_Expression {
		Ast_UnaryExpression() { type = AST_UNARY; }

		int op = AST_UNARY_NONE;
	};

	struct Ast_Statement : public Ast {
		Ast_Statement() { type = AST_STATEMENT; }
		~Ast_Statement() { delete expr; delete id; }

		Ast_Identifier* id = nullptr;
		Ast_Expression* expr = nullptr;
	};

	struct Ast_Assignment : public Ast_Statement {
		Ast_Assignment() { type = AST_ASSIGNMENT; }
	};

	struct Ast_Procedure : public Ast_Statement {
		Ast_Procedure() { type = AST_PROCEDURE; }
		~Ast_Procedure() { for (size_t i = 0; i < args.size(); i++) delete args[i]; }

		std::vector<Ast_Identifier*> args;
	};

	struct Ast_Script : public Ast {
		Ast_Script() { type = AST_SCRIPT; }
		~Ast_Script() { for (size_t i = 0; i < procedures.size(); i++) delete procedures[i]; }

		std::vector<Ast_Statement*> procedures;
	};

#define START 0
#define LEFT 1
#define RIGHT 2

	class Parser {
	public:
		Parser() = default;
		Parser(Lexer* lexer);
		~Parser();
		void Destroy();

		void Run();
		void Visualize();
		void VisualizeExpression(Ast_Expression* expr, int indent = 1);
		void Ident(int indent);

		Ast* DefaultAst(Ast* ast);
		Token* Peek();
		Token* PeekOff(int off);
		Token* Next();
		void Match(int type);
	private:
		std::vector<std::unordered_map<Ast_Identifier, double>> symbols;
		Lexer* lexer = nullptr;
		Ast_Script* root = nullptr;
		uint32_t index = 0;
	private:
		Ast_Statement* ParseStatement();
		Ast_Identifier* ParseId();
		Ast_Expression* ParseExpression(int prec = 1);
		Ast_Expression* ParseUnary();
		Ast_Expression* ParsePrimary();
		Ast_BinaryExpression* ParseBinary();
		Ast_ProcedureCall* ParseProcedureCall();
	};
}

#endif // !PARSER_H
