#pragma once
#ifndef LP_SOLVER_H_
#define LP_SOLVER_H_


#include <vector>
#include <unordered_map>
#include <string>

//#include "lp_lib.h"

typedef long long int64;

namespace Solver {

	class LPConstraint;
	class LPObject;
	class LPInterface;
	class LPVariable;
	class LPErrorandLog;

	class LPSolver {
	public:
		LPSolver(const std::string& name);
		~LPSolver() {};

		// Get the name of model
		std::string Name() const {
			return name_;
		}

		// The type of underlying solver;
		enum UnderlyingSolverType {

		};
		// Get the typr of underlying solver
		UnderlyingSolverType type_of_undering_solver() const { 
			return type_of_underlying_solver;
		}

		void Clear();

		//define infiinity, +¡Þ = LPSolver::infinity() and -¡Þ = - LPSolver::infinity()
		static double infinity() { return std::numeric_limits<double>::infinity(); }

		/*     Variables     */
		// ---  Get Variables Info  ---
		// Return the number of variables
		int GetNumberOfVariables() const { return variables_.size(); }
		//Get the array of variables
		const std::vector<LPVariable> & GetVariables() const { return variables_; }
		
		// ---  Creat Single Variables  ---
		// Create a continuous variable with default parameters
		LPVariable* CreatContinuousVar();
		// Create a continuous variable with specific parameters
		LPVariable* CreatContinuousVar(double lb, double ub, std::string name );
		// Create an integer variable with default parameters
		LPVariable* CreatIntegerVar();
		// Create an integer variable with specific parameters
		LPVariable* CreatIntegerVar(double lb, double ub, std::string name);
		// Create a binary variable with default parameters
		LPVariable* CreatBinaryVar();
		//Create a binary variable with specific parameters
		LPVariable* CreatBinaryVar(std::string name);

		// ---  Creat an Array of variables  ---
		// Creat an array of continuous variables with default parameters
		std::vector<LPVariable*> CreatContinuousVarArray();
		// Creat an array of continuous variables with specific parameters
		std::vector<LPVariable*> CreatContinuousVarArray(int num, double lb, double ub, std::string name_prefix);
		// Creat an array of integer variables with default parameters
		std::vector<LPVariable*> CreatIntegerVarArray();
		// Creat an array of integer variables with specific parameters
		std::vector<LPVariable*> CreatIntegerVarArray(int num, double lb, double ub, std::string name_prefix);
		// Creat an array of binary variables with default parameters
		std::vector<LPVariable*> CreatBinaryVarArray();
		// Creat an array of binary variables with specific parameters
		std::vector<LPVariable*> CreatBinaryVarArray(int num, std::string name_prefix);

		/*     Constraints     */
		// ---  Get Constraints Info  ---
		// Return the number of constraints
		int GetNumberOfConstraints() const { return constraints_.size(); }
		// Return the array of constraints
		const std::vector<LPConstraint> & GetConstraints() const { return constraints_; }

		// --- Creat Single Constraint --- //
		// Creat a constraint with default ranges
		LPConstraint* MakeConstraint();
		// Creat a constraint with specific ranges
		LPConstraint* MakeConstraint(double lb, double ub);

		// --- Creat a set of constraints ---
		// Creat a set of constraints with same ranges
		std::vector<LPConstraint*> MakeConstraintMarix(double lb, double ub);

		/*     Object     */
		// Return the Object
		const std::vector<LPObject> & GetObject_() const { return object_; }
		// Create an object
		LPObject* CreateObject();

		/*     Solve     */
		// Status of solution
		enum StatusofLPSolution { 
			OPTIMAL,        // optimal.
			FEASIBLE,       // feasible, or stopped by limit.
			INFEASIBLE,     // proven infeasible.
			UNBOUNDED,      // proven unbounded.
			ABNORMAL,       // abnormal, i.e., error of some kind.
			MODEL_INVALID,  // the model is trivially invalid (NaN coefficients, etc).
			NOT_SOLVED = 6  // not been solved yet.
		};
		// Solve
		StatusofLPSolution Solve();
		// Get the status of lp_solver
		StatusofLPSolution GetSolutionStatus() {
			return status_of_solution;
		}

		/*     Export Model to File     */
		bool ExportModelAsLpFormat() const;  //return true if exporting sucessufully

		/*     Limits     */
		// Set time limit
		void set_time_limit(int64 t_limit) {
			time_limit = t_limit;
		}
		// Get time limit
		int64 get_time_limit() const { return time_limit; }
		// Set iter limit
		void set_iter_limit(int64 iter_limit) {
			iterations_limit = iter_limit;
		}
		// Get iter limit
		int64 get_iter_limit() const { return iterations_limit; }

	private:
		bool InfeasibleConstraint();
		bool HasIntegerVariables();
		const std::string name_;
		
		//Variables
		std::vector<LPVariable> variables_;
		std::unordered_map<std::string, int> variable_name_index;

		//Constraints
		std::vector<LPConstraint> constraints_;
		std::unordered_map<std::string, int> constraints_name_index;

		//Objective
		std::vector<LPObject> object_;

		//Limits;
		int64 time_limit;
		int64 iterations_limit;

		//Status of the solution
		const StatusofLPSolution status_of_solution;

		//Type of underlying solver
		const UnderlyingSolverType type_of_underlying_solver;
	};
}

#endif