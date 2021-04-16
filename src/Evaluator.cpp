#include "Evaluator.h"
#include <cmath>
#include <sstream>
#include <string>
#include <iostream>
using namespace std;


bool Evaluator::initialize()
{
	initFunctionNames();
	constants.clear();
	parsedExpression.clear();
	nTerminals = 0;

	return true;
}


void Evaluator::initFunctionNames()
{
	termNames.clear();

	funcNames.clear();
	funcNames.push_back("+");
	funcNames.push_back("-");
	funcNames.push_back("*");
	funcNames.push_back("/");
	funcNames.push_back("sin");
	funcNames.push_back("cos");
	funcNames.push_back("pos");
	funcNames.push_back("ifpos");
	funcNames.push_back("ifgt");
	funcNames.push_back("min");
	funcNames.push_back("max");
	funcNames.push_back("avg");
	funcNames.push_back("sqrt");

	nFunctions = funcNames.size();
}


bool Evaluator::addTerminal(std::string termName)
{
	termNames.push_back(termName);
	nTerminals = termNames.size();

	return true;
}


bool Evaluator::parseExpression(std::string expression, uint size)
{
	if(size > parsedExpression.size())
		parsedExpression.resize(size);

	constants.clear();

	stringstream ss(expression);
	std::string strPrimitive;

	// read whole tree expression
	for (uint index = 0; index < size; index++) {
		ss >> strPrimitive;
		// check function names
		bool found = false;
		for (uint iPrim = 0; iPrim < nFunctions; iPrim++) {
			if (strPrimitive == funcNames[iPrim]) {
				parsedExpression[index] = iPrim;
				found = true;
				break;
			}
		}
		if(found)
			continue;

		// check terminals
		for (uint iPrim = 0; iPrim < nTerminals; iPrim++) {	
			if (strPrimitive == termNames[iPrim]) {
				parsedExpression[index] = iPrim + Evaluator::TERMINALS;
				found = true;
				break;
			}
		}
		if(found)
			continue;

		// constants
		if (strPrimitive.substr(0, 2) == "D_") {
			stringstream cs(strPrimitive.substr(2));
			double cValue;
			cs >> cValue;
			constants.push_back(cValue);
			parsedExpression[index] = constants.size() - 1 + Evaluator::CONSTANTS;
		}
	}

	return true;
}


double Evaluator::executeParsedExpression(uint sample, uint iTree)
{
	iSample = sample;
	position = -1;
	
	return execute();
}


double Evaluator::execute()
{
	position++;

	switch(parsedExpression[position]) {
	case ADD:
		return execute() + execute();
		break;
	case SUB:
		return execute() - execute();
		break;
	case MUL:
		return execute() * execute();
		break;
	case DIV: {	
		double d1 = execute();
		double d2 = execute();
		if(fabs(d2) < 0.000000001)
			return 1;
		else
			return d1/d2;
		break;
				}
	case SIN:
		return sin(execute());
		break;
	case COS:
		return cos(execute());
		break;
	case POS: {
		double d = execute();
		return (d > 0) ? d : 0;
		break;
				}
	case IFPOS: {
		double v = execute();
		double d1 = execute();
		double d2 = execute();
		return (v >= 0) ? d1 : d2;
		break;
				}
	case IFGT: {
		double v1 = execute();
		double v2 = execute();
		double d1 = execute();
		double d2 = execute();
		return (v1 > v2) ? d1 : d2;
		break;
				}
	case MIN: {	
		double d1 = execute();
		double d2 = execute();
		if(d1 < d2)
			return d1;
		else
			return d2;
		break;
				}
	case MAX: {	
		double d1 = execute();
		double d2 = execute();
		if(d1 > d2)
			return d1;
		else
			return d2;
		break;
				}
	case AVG: {	
		double d1 = execute();
		double d2 = execute();
		return (d1 + d2)/2;
		break;
				}
	case SQRT: {
		double d = execute();
		return (d > 0) ? sqrt(d) : 0;
		break;
				}
	default:
		if(parsedExpression[position] >= CONSTANTS)
			return constants[parsedExpression[position] - CONSTANTS];
		else
			//return (*terminalValues[parsedExpression[position] - TERMINALS])	[iSample];
			return (*data)[iSample][parsedExpression[position] - TERMINALS];
		break;
	}
}
