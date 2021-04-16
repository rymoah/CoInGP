#ifndef Sqrt_h
#define Sqrt_h

#include <ecf/tree/Primitive.h>

namespace Tree {
namespace Primitives {

/**
 * \ingroup tree genotypes primitives
 * \brief Sqrt function primitive (Tree genotype)
 */
class Sqrt : public Primitive
{
public:
	Sqrt(void);
	void execute(void* result, Tree& tree);
	~Sqrt(void);
};

Sqrt::Sqrt(void)
{
	nArguments_ = 1;
	name_ = "sqrt";
}


Sqrt::~Sqrt(void)
{	}


void Sqrt::execute(void* result, Tree &tree)
{
	double& arg = *(double*)result;
    getNextArgument(&arg, tree);
	if (arg > 0) {
		arg = sqrt(arg);
	} else {
		arg = 0;
	}
}


}
}

#endif 