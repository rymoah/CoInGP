#ifndef Evaluator_h
#define Evaluator_h

#include <vector>
#include <string>
typedef unsigned int uint;


class Evaluator
{
public:
	std::vector< std::vector<double> > *data;
	std::vector<double> constants;
	uint nSamples;
	uint nTerminals;
	uint nFunctions;

	// enumerated functions; names are defined in initFunctionNames()
	enum Functions {ADD, SUB, MUL, DIV, SIN, COS, POS, IFPOS, IFGT, MIN, MAX, AVG, SQRT};
	static const uint TERMINALS = 1000;
	static const uint CONSTANTS = 2000;

	std::vector<std::string> funcNames, termNames;
	std::vector<uint> parsedExpression;
	int position;
	uint iExpression;
	uint iSample;

	bool initialize();
	void initFunctionNames();
	double executeParsedExpression(uint sample, uint iTree = 0);
	double execute();
	bool addTerminal(std::string);
	bool setTerminalValues(std::string, std::vector<double>& values);
	bool parseExpression(std::string, uint size);
};

#endif // Evaluator_h
