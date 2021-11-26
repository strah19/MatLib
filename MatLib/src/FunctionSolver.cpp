#include "FunctionSolver.h"

namespace MatLib {
	FunctionSolver::FunctionSolver(Parser* parser) : Interpreter(parser) { }

	void FunctionSolver::Solve() {
		if (parser->Root()) {
			auto root = parser->Root();
			for (auto& proc : root->procedures) {
				switch (proc->type) {
				case AST_ASSIGNMENT:
					auto assign = static_cast<Ast_Assignment*>(proc);
					printf("Assignment: %s\n", assign->id->id.c_str());
					if (assign->expr) {
						printf("Answer: %f\n", SolveExpression(assign->expr));
					}
					break;
				}
			}
		}
	}
}