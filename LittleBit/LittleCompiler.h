#pragma once

#include <utility>
#include <string>
#include <vector>
#include <stack>
#include <list>
#include <map>

#include "LittleIntern.h"
#include "LittleCode.h"

#include "JumpMark.h"
#include "Scope.h"
namespace LilBit
{
	//Verifies and compiles code into executable LittleBit bytecode
	class Compiler
	{
		public:
		Compiler();

		bool hasLog();
		void nextLog();
		std::string& getLog();
		void postLog(std::string message);

		//Inserts a branch which jumps on *not null* for a SM value
		bool ifNull(ID smVar, ID endJumpID);
		//Inserts a branch which jumps on *null* for a SM value
		bool ifValue(ID smVar, ID endJumpID);
		//Inserts a branch which jumps on *-ve* for a SM twos complement value
		bool ifPositive(ID smVar, ID endJumpID);
		//Inserts a branch which jumps on *+ve* for a SM twos complement value
		bool ifNegative(ID smVar, ID endJumpID);

		//Inserts a jump command by name. Always successful until labels are bulk checked
		void jumpTo(std::string labelName);
		//Inserts a jump command. Returns if successful
		bool jumpTo(ID jumpID);
		//Inserts a generic jump command. Returns if successful
		bool jumpTo(JumpMark mark);
		//Inserts a jump command towards the designated continue a set number of scopes backwards. Returns if successful
		bool doContinue(size_t scopeDif);
		//Inserts a jump command towards the designated break a set number of scopes backwards. Returns if successful
		bool doBreak(size_t scopeDif);

		//Beginning of a new scope
		void newScope();
		//Declares a label at this point of the program. Returns if already declared. Overrides virtual location anyway
		bool declareLabel(std::string name);
		//Declares a type. Returns the ID of the new declaration or a found existing one
		ID declareType(std::string name);
		//Creates an alias for a type. Returns if successful
		bool declareTypeAlias(ID knownId, std::string name);

		//Declares the function with the known ID. Returns if successful
		bool assertFunction(ID id, std::string name, const std::vector<std::string>& params);

		private:
		//Splits off a new code packet
		void newRun();
		//Declares/fetches the jump ID associated with a label name
		ID promiseJumpDest(std::string destName);
		//Declares a new jump and returns its ID
		ID promiseJumpDest();

		std::stack<std::string> log; //Tracks errors and issues

		//The scope where the code presides. The end causes the compiler to finalise
		//Exists so a meta scope can exist on the same framework!
		size_t exitLayer;
			  
		//TODO UH THIS SOLUTION *MIGHT* WORK
		//Tracks the hierarchy
		std::vector<Scope> scopes;
		//Code packets between jump instructions and destinations
		//(will be replaced in the future with an analytical system instead of direct)
		std::vector<LilBit::Code> runs; 

		//Jumps at virtual locations. Always in order
		std::vector<std::pair<size_t, LilBit::JumpMark>> jumps;

		size_t virtualLocation; //Represents the current virtual location
		//Pairs label name to a registered jump ID
		std::map<std::string, ID> jumpLabels;
		//Pairs jump ID to virtual location
		std::vector<size_t> jumpIndexer;

		//The earliest free SM location
		size_t lastFreeStatic;
		//Represents all SM spaces, if the space is taken and its type
		std::tuple<bool, ID>* statics;
		//Maps visible variable name to SM space
		std::map<std::string, ID> staticVariables;

		ID typeProg; //Represents the next available unique type ID
		//Pairs a type name with its associated ID and scope. Aliases inherit the same ID
		std::map<std::string, std::pair<ID, size_t>> typeDec;

		//Pairs function name with parameter list and then pairs that to a unique ID
		std::map<std::string, std::map<std::vector<ID>, ID>> functionDec;

	};
}

