#pragma once

#include <list>

#include "LittleIntern.h"
#include "LittleCompiler.h"
namespace LilBit
{
	class Scope
	{
		public:

		private:
		ID breakJumpID; //The jump location at the end of this scope
		ID continueJumpID; //The jump location which logically begins this scope

		std::list<ID> staticVariables; //SM ID's of variables tied to this scope
		std::list<std::string> staticVarDecs; //Named SM variables in this scope
		
		std::list<std::string> typeDecs; //Names of types in this scope
	};
}

