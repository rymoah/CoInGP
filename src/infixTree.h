#include <ecf/ECF.h>

//
// ispis Tree u infix formatu
//
uint showTree(string& output, Tree::Tree* tree, uint iNode = 0, uint prefix = 0)
{
	Tree::PrimitiveP prim = tree->at(iNode)->primitive_;
	int arity = prim->getNumberOfArguments();

	if(arity == 0) {
		output += prim->getName();
	}
	else if(prim->getName() == "P2") {
		output += "P2(";
			iNode++;
			iNode = showTree(output, tree, iNode, prefix);
		output += ", ";
			iNode++;
			iNode = showTree(output, tree, iNode, prefix);
		output += ")";
	}
	else if(prim->getName() == "avg" || prim->getName() == "min" || prim->getName() == "max") {
		output += prim->getName() + "(";
			iNode++;
			iNode = showTree(output, tree, iNode, prefix);
		output += ", ";
			iNode++;
			iNode = showTree(output, tree, iNode, prefix);
		output += ")";
	}
	else if(prim->getName() == "ADD" ||
		prim->getName() == "AND" ||
		prim->getName() == "NAND" ||
		prim->getName() == "AND2" ||
		prim->getName() == "XOR" ||
		prim->getName() == "XNOR" || 
		prim->getName() == "NOR" ||
		prim->getName() == "OR") {
		output += "(";
			iNode++;
			iNode = showTree(output, tree, iNode, prefix);
		output += " " + prim->getName() + " ";
			iNode++;
			iNode = showTree(output, tree, iNode, prefix);
		output += ")";
	}
	else if(prim->getName() == "MUL") {
		output += "(";
			iNode++;
			iNode = showTree(output, tree, iNode, prefix);
		output += " * ";
			iNode++;
			iNode = showTree(output, tree, iNode, prefix);
		output += ")";
	}
	else if(prim->getName() == "NOT") {
		output += "~";
		output += "(";
		for(int child = 0; child < arity; child++) {
			iNode++;
			iNode = showTree(output, tree, iNode, prefix);
		}
		output += ")";
	}
	else if(prim->getName() == "IF") {
		output += "IF(";
			iNode++;
			iNode = showTree(output, tree, iNode, prefix);
		output += ", ";
			iNode++;
			iNode = showTree(output, tree, iNode, prefix);
		output += ", ";
			iNode++;
			iNode = showTree(output, tree, iNode, prefix);
		output += ")";
	}
	else if(arity == 2) {
		output += "(";
			iNode++;
			iNode = showTree(output, tree, iNode, prefix);
		output += " "  + prim->getName() + " ";
			iNode++;
			iNode = showTree(output, tree, iNode, prefix);
		output += ")";
	}
	else if(arity == 1) {
		output += prim->getName();
		output += "(";
		for(int child = 0; child < arity; child++) {
			iNode++;
			iNode = showTree(output, tree, iNode, prefix);
		}
		output += ")";
	}
	return iNode;
}
