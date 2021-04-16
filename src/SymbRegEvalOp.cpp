#include <cmath>
#include <boost/math/special_functions/fpclassify.hpp> // isnan, isinf
#include <ecf/ECF.h>
#include "SymbRegEvalOp.h"
#include "ReadData.h"
#include "infixTree.h"

// from WriteBest.h
extern bool evaluateVerbose;


void SymbRegEvalOp::registerParameters(StateP state)
{
	state->getRegistry()->registerEntry("input_file", (voidP)(new std::string), ECF::STRING);
	state->getRegistry()->registerEntry("linear_scaling", (voidP) (new uint(0)), ECF::UINT);
	state->getRegistry()->registerEntry("error_weights_file", (voidP) (new std::string), ECF::STRING);
	state->getRegistry()->registerEntry("error_metric", (voidP) (new std::string), ECF::STRING);
	state->getRegistry()->registerEntry("data_skip", (voidP) (new uint(1)), ECF::UINT);
	state->getRegistry()->registerEntry("data_offset", (voidP) (new uint(0)), ECF::UINT);
}


void SymbRegEvalOp::createTerminals(StateP state)
{
	static std::string configTerminals = *((std::string*) state->getGenotypes()[0]->getParameterValue(state, "terminalset").get());
	//configTerminals = *((std::string*) state->getRegistry()->getEntry("Tree.terminalset").get());

	varNames.clear();
	std::string terminalSet;
	for(uint i = 0; i < nVariables; i++) {
		varNames.push_back ("x" + uint2str(i + 1));
		terminalSet += varNames[i] + " ";
	}

	// set terminal names
	terminalSet = configTerminals + " " + terminalSet;
	state->getGenotypes()[0]->setParameterValue(state, "terminalset", (voidP) new std::string(terminalSet));
	//state->getRegistry()->modifyEntry("Tree.terminalset", (voidP) new std::string(terminalSet));

	// reinitialize population with updated terminals
	Tree::Tree* hometree = (Tree::Tree*) state->getGenotypes()[0].get();
	hometree->primitiveSet_ = Tree::PrimitiveSetP();
	state->getPopulation()->initialize(state);
}


bool SymbRegEvalOp::initialize(StateP state)
{
	state_ = state;
	recordResults = false;
	results.clear();

	voidP sptr = state->getRegistry()->getEntry("data_skip");
	uint skip = *((uint*)sptr.get());
	sptr = state->getRegistry()->getEntry("data_offset");
	uint offset = *((uint*)sptr.get());
	sptr = state->getRegistry()->getEntry("linear_scaling");
	uint scaling = *((uint*)sptr.get());
	linearScaling = false;
	if(scaling != 0)
		linearScaling = true;

	std::string dataFile = *((std::string*) state->getRegistry()->getEntry("input_file").get());
	// read from file into Evaluator data
	if (!readDataFromFile(data, dataFile, offset, skip)) {
		return false;
	}

	nSamples = data.size();
	nVariables = data[0].size() - 1;
	results.resize(nSamples);

	// a) create terminal names for use with canonical GP
	createTerminals(state);

	// b) read terminal names from config
	//varNames.clear();
	//std::string terminals = *((std::string*) state->getGenotypes()[0]->getParameterValue(state, "terminalset").get());
	//std::stringstream ss(terminals);
	//std::string terminal;
	//for(uint i = 0; i < nVariables; i++) {
	//	ss >> terminal;
	//	varNames.push_back (terminal);
	//}

	// initialize evaluator
	eval.data = &data;
	eval.initialize();
	for(uint i = 0; i < nVariables; i++) {
		eval.addTerminal(varNames[i]);
	}

	return true;
}


FitnessP SymbRegEvalOp::evaluate(IndividualP individual)
{
	if(linearScaling == true)
		return evaluateUsingLinearScaling(individual);

	// we try to minimize the function value, so we use FitnessMin fitness (for minimization problems)
	FitnessP fitness (new FitnessMin);

	// get the genotype we defined in the configuration file
	Tree::Tree* tree = (Tree::Tree*) individual->getGenotype().get();

	// output tree expression to string
	std::stringstream sValue;
	for(uint i = 0; i < tree->size(); i++) {
		sValue << (*tree)[i]->primitive_->getName() << " ";
	}
	// send expression to evaluator
	eval.parseExpression(sValue.str(), tree->size());

	double value = 0;
	double result;
	// evaluating data from input file
	for (uint i = 0; i < nSamples; i++) {
		//// set only defined variables (x1, x2, ...)
		//for(uint term = 0; term < nVariables; term++) {
		//	tree->setTerminalValue(varNames[term], &data[i][term]);
		//}

		//// get the f value of the current tree
		//tree->execute(&result);

		result = eval.executeParsedExpression(i);

		// clip the value to [0, 255]
		result = (int) abs(result);
		if(result > 255)
			result = 255;

		if(recordResults)
			results[i] = result;

		// add the absolute difference
		//value += fabs(eval.data[i][nVariables] - result);
		// or squared error
		double error = fabs(data[i][nVariables] - result) * fabs(data[i][nVariables] - result);
		
		value += error;
	}

	value /= nSamples;	   // MSE
	value = sqrt(value);   // RMSE

	fitness->setValue(value);

	if(evaluateVerbose) {
		stringstream ss;
		string infix;
		showTree(infix, tree);
		ss << infix << endl;
		ECF_LOG(state_, 1, ss.str());
	}

	return fitness;
}


FitnessP SymbRegEvalOp::evaluateUsingLinearScaling(IndividualP individual)
{
	FitnessP fitness (new FitnessMin);

	// get the genotype we defined in the configuration file
	Tree::Tree* tree = (Tree::Tree*) individual->getGenotype().get();

	// output tree expression to string
	std::stringstream sValue;
	for(uint i = 0; i < tree->size(); i++) {
		sValue << (*tree)[i]->primitive_->getName() << " ";
	}
	// send expression to evaluator
	eval.parseExpression(sValue.str(), tree->size());

	//average value of y and t
	double mean_y = 0, mean_t = 0, sum_yt = 0, sum_sqr_y = 0;

	for (uint i = 0 ; i < nSamples ; i++)
	{
		//// set only defined variables (x1, x2, ...)
		//for(uint term = 0; term < nVariables; term++) {
		//	tree->setTerminalValue(varNames[term], &data[i][term]);
		//}

		// get the y value of the current tree
		double y;
		//tree->execute(&y);
		y = eval.executeParsedExpression(i);

		// clip the value to [0, 255]
		y = (int) abs(y);
		if(y > 255)
			y = 255;

		mean_y += y;
		double t = data[i][nVariables];
		mean_t += t;
		sum_yt += t*y;
		sum_sqr_y += y*y;
	}

	mean_y /= nSamples;
	mean_t /= nSamples;

	//coefficients for linear regression
	double b = (sum_yt - nSamples*mean_y*mean_t)/(sum_sqr_y - nSamples*mean_y*mean_y);
	double a = mean_t - b*mean_y;

	bool isNaN = boost::math::isnan(a) || boost::math::isnan(b);
	bool isInf = boost::math::isinf(a) || boost::math::isinf(b);
	if (isNaN || isInf) {
		a = 0;
		b = 1;
//		fitness->setValue(1e14);
//		return fitness;
	}

	double value = 0;
	double var_of_y = 0; //variance of variable result

	for(uint i = 0; i < nSamples; i++) {
		//// set only defined variables (x1, x2, ...)
		//for(uint term = 0; term < nVariables; term++) {
		//	tree->setTerminalValue(varNames[term], &data[i][term]);
		//}

		double y;
		//tree->execute(&y);
		y = eval.executeParsedExpression(i);

		// clip the value to [0, 255]
		y = (int) abs(y);
		if(y > 255)
			y = 255;

		var_of_y += (y-mean_y)*(y-mean_y);
		double result = a + b*y;
		result = (int) result;

		if(recordResults)
			results[i] = result;

		// add the absolute difference
		//value += fabs(eval.data[i][nVariables] - result);
		// or squared error
		double error = fabs(data[i][nVariables] - result) * fabs(data[i][nVariables] - result);
		value += error;
	}

	value /= nSamples;	   // MSE
	value = sqrt(value);   // RMSE

	var_of_y /= nSamples-1;

	if (evaluateVerbose) {
		stringstream ss;
		string infix;
		showTree(infix, tree);
		ss << infix << endl;
		ss << "Linear scaling parameters: scale=" << b << " offset=" << a << std::endl;
		ECF_LOG(state_, 1, ss.str());
	}

	fitness->setValue(value);

	return fitness;
}

