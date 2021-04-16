#include <iostream>
#include <vector>
#include <ecf/ECF.h>
#include "SymbRegEvalOp.h"
#include "WriteBest.h"
#include "Sqrt.h"
#include "Avg.h"
using namespace std;


int main(int argc, char **argv)
{
	StateP state (new State);

	// set the evaluation operator
	SymbRegEvalOpP eval(new SymbRegEvalOp);
	state->setEvalOp(eval);

    // add writeout of best individual each generation
    state->addOperator((OperatorP) new WriteBest);

    TreeP tree = (TreeP) new Tree::Tree;
    Tree::Primitives::PrimitiveP sqrtPrimitive = (Tree::Primitives::PrimitiveP) new Tree::Primitives::Sqrt;
    tree->addFunction(sqrtPrimitive);
    Tree::Primitives::PrimitiveP avgPrimitive = (Tree::Primitives::PrimitiveP) new Tree::Primitives::Avg;
    tree->addFunction(avgPrimitive);
    state->addGenotype(tree);

	if(!state->initialize(argc, argv))
		return 1;

	// read and evaluate individual from file in 3rd agrument
	if(argc >= 3) {
		XMLNode xInd = XMLNode::parseFile(argv[2], "Individual");
		IndividualP ind = (IndividualP) new Individual(state);
		ind->read(xInd);

		evaluateVerbose = true;
		if(argc == 4)
			eval->recordResults = true;
		ind->fitness = state->getAlgorithm()->evalOp_->evaluate(ind);
		cout << ind->toString();

		// xval output
		ofstream outfile("xval.txt", ios_base::app);
		outfile << ind->fitness->getValue() << endl;
		outfile.close();

		if(argc == 4) {
			ofstream outFile(argv[3]);
			for(uint i = 0; i < eval->results.size(); i++)
				outFile << eval->results[i] << endl;
			outFile.close();
		}

		return 0;
	}

	state->run();

	return 0;
}

