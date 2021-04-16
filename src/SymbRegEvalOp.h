#ifndef SymbRegEvalOp_h
#define SymbRegEvalOp_h

#include "Evaluator.h"


class SymbRegEvalOp : public EvaluateOp
{
public:
	FitnessP evaluate(IndividualP individual);
	FitnessP evaluateUsingLinearScaling(IndividualP ind);
	bool initialize(StateP);
	void registerParameters(StateP);
	void createTerminals(StateP);

	std::vector<double> domain;
	std::vector<double> results;
	std::vector< std::vector<double> > data;
	std::vector<string> varNames;
	uint nSamples;
	uint nVariables;
	StateP state_;
	bool linearScaling;
	bool recordResults;

	Evaluator eval;
};
typedef boost::shared_ptr<SymbRegEvalOp> SymbRegEvalOpP;

#endif // SymbRegEvalOp_h
