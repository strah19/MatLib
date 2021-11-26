#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "Parser.h"

namespace MatLib {
	class Interpreter {
	public:
		Interpreter() = default;
		Interpreter(Parser* parser);

		double SolveExpression(Ast_Expression* expr);
	protected:
		Parser* parser; 
	};
}

#endif // !INTERPRETER_H
