#pragma once

#include <utility>
#include <functional>
#include <string>
#include <vector>
#include <map>

#include "LittleIntern.h"
#include "LittleCompiler.h"

namespace LilBit
{
	class FunctionWrangler
	{
		public:
		FunctionWrangler();

		LilBit::FunctionWrangler& addFunction(float freq, std::string name, std::vector<std::string> paramTypes, std::function<Func> function);

		//Attempts to register functions, returns if successful
		bool bakeFunctions(LilBit::Intern& interpreter, LilBit::Compiler& compiler);

		private:
		//Function frequency paired with function name, function param types and the function itself
		std::map<float, std::tuple<std::string, std::vector<std::string>, std::function<Func>>> functionOrd;
		ID nextFunc;
	};
}

