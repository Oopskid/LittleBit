#include "FunctionWrangler.h"

LilBit::FunctionWrangler::FunctionWrangler()
{
	nextFunc = 0;
}

LilBit::FunctionWrangler& LilBit::FunctionWrangler::addFunction(float freq, std::string name, std::vector<std::string> paramTypes, Functor function)
{
	functionOrd.insert(std::make_pair(freq, std::make_tuple(name, paramTypes, function))).second;
	return *this;
}

bool LilBit::FunctionWrangler::bakeFunctions(LilBit::Intern& interpreter, LilBit::Compiler& compiler)
{
	//Functions in order of frequency
	for (auto it = functionOrd.rbegin(); it != functionOrd.rend(); it++)
	{
		//Register with the compiler
		if (compiler.assertFunction(nextFunc, std::get<0>(it->second), std::get<1>(it->second)))
		{
			//Formally register with the interpreter
			interpreter.registerFunc(nextFunc, std::get<2>(it->second), std::get<1>(it->second).size());

			//Next function
			nextFunc++;
		}
		else { return false; }
	}

	functionOrd.clear();

	return true;
}
