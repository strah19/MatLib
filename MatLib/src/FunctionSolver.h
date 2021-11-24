#ifndef FUNCTION_SOLVER_H
#define FUNCTION_SOLVER_H

#include "Interpreter.h"

namespace MatLib {
	class FunctionSolver : public Interpreter {
	public:
		FunctionSolver() = default;
		FunctionSolver(Parser* parser);

		void Solve();
	private:

	};
}

#endif // !FUNCTION_SOLVER_H
